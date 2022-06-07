/**
 * \file main.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * Adapted from:
 * https://github.com/CMUAbstract/alpaca-oopsla2017/blob/master/src/main_ar_dino.c
 */

#include "immortality.h"
#include <stdio.h>

//#define PORT_CONTROL

#ifdef PF_SIMULATION
#define min_pf 10
__fram int timer_count = 500;
__fram unsigned int volatile random_num; // read temperature value
__fram int mod5_rand;
#endif

unsigned count = 0;

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t z;
} threeAxis_t;

// Number of samples to discard before recording training set
#define NUM_WARMUP_SAMPLES 3

#define ACCEL_WINDOW_SIZE 3
#define MODEL_SIZE 16
#define SAMPLE_NOISE_FLOOR 10 // TODO: made up value

// Number of classifications to complete in one experiment
#define SAMPLES_TO_COLLECT 128
typedef threeAxis_t accelReading;
typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];

typedef struct {
  unsigned meanmag;
  unsigned stddevmag;
} features_t;

typedef enum {
  CLASS_STATIONARY,
  CLASS_MOVING,
} class_t;

typedef struct {
  features_t stationary[MODEL_SIZE];
  features_t moving[MODEL_SIZE];
} model_t;

typedef enum {
  MODE_IDLE = 3,
  MODE_TRAIN_STATIONARY = 2,
  MODE_TRAIN_MOVING = 1,
  MODE_RECOGNIZE = 0, // default
} run_mode_t;

typedef struct {
  unsigned totalCount;
  unsigned movingCount;
  unsigned stationaryCount;
} stats_t;

#ifndef __clang__
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

immortalc_fn_idempotent uint32_t mult16(uint16_t a, uint16_t b) {
  // NOTE: We have to hardcode addresses, because we ca neither access
  // definitions from the msp430.h header inside the assembly, nor
  // parametrize the assembly snippet since function is naked. A non-naked
  // function with parametrized snipped turned out to be horribly inefficient
  // (the result construction needs a shift: 8 instructions...).

  // NOTE: The registers are dependent on compiler calling convention.

  // Load OP1 and OP2 into the hardware multiplier and read result.

#ifdef __MSP430__
  __asm__ volatile("MOV R12, &0x04C0\n"
                   "MOV R13, &0x04C8\n"
                   "MOV &0x04CA, R12\n"
                   "MOV &0x04CC, R13\n"
/*                   "RETA\n"*/);
#else
  return (uint32_t)a * (uint32_t)b;
#endif
}

/* Square root by Newton's method */
immortalc_fn_idempotent uint16_t sqrt16(uint32_t x) {
  uint16_t hi = 0xffff;
  uint16_t lo = 0;
  uint16_t mid = ((uint32_t)hi + (uint32_t)lo) >> 1;
  uint32_t s = 0;

  while (s != x && hi - lo > 1) {
    mid = ((uint32_t)hi + (uint32_t)lo) >> 1;
    s = mult16(mid, mid);
    if (s < x)
      lo = mid;
    else
      hi = mid;
  }

  return mid;
}

immortalc_fn_idempotent void accel_sample(unsigned seed, accelReading *result) {
  result->x = (seed * 17) % 85;
  result->y = (seed * 17 * 17) % 85;
  result->z = (seed * 17 * 17 * 17) % 85;
}

void acquire_window(accelWindow window) {
  unsigned seed = 1;
  accelReading sample;
  unsigned samplesInWindow = 0;

  while (samplesInWindow < ACCEL_WINDOW_SIZE) {
    accel_sample(seed, &sample);
    seed++;
    window[samplesInWindow++] = sample;
  }
}

void transform(accelWindow window) {
  unsigned i = 0;

  for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
    accelReading *sample = &window[i];

    if (sample->x < SAMPLE_NOISE_FLOOR || sample->y < SAMPLE_NOISE_FLOOR ||
        sample->z < SAMPLE_NOISE_FLOOR) {

      sample->x = (sample->x > SAMPLE_NOISE_FLOOR) ? sample->x : 0;
      sample->y = (sample->y > SAMPLE_NOISE_FLOOR) ? sample->y : 0;
      sample->z = (sample->z > SAMPLE_NOISE_FLOOR) ? sample->z : 0;
    }
  }
}

void featurize(features_t *features, accelWindow aWin) {
  accelReading mean;
  accelReading stddev;

  mean.x = mean.y = mean.z = 0;
  stddev.x = stddev.y = stddev.z = 0;
  int i;

  for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
    mean.x += aWin[i].x; // x
    mean.y += aWin[i].y; // y
    mean.z += aWin[i].z; // z
  }

  mean.x >>= 2;
  mean.y >>= 2;
  mean.z >>= 2;

  for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
    stddev.x +=
        aWin[i].x > mean.x ? aWin[i].x - mean.x : mean.x - aWin[i].x; // x
    stddev.y +=
        aWin[i].y > mean.y ? aWin[i].y - mean.y : mean.y - aWin[i].y; // y
    stddev.z +=
        aWin[i].z > mean.z ? aWin[i].z - mean.z : mean.z - aWin[i].z; // z
  }

  stddev.x >>= 2;
  stddev.y >>= 2;
  stddev.z >>= 2;

  unsigned meanmag = mean.x * mean.x + mean.y * mean.y + mean.z * mean.z;
  unsigned stddevmag =
      stddev.x * stddev.x + stddev.y * stddev.y + stddev.z * stddev.z;

  features->meanmag = sqrt16(meanmag);
  features->stddevmag = sqrt16(stddevmag);
}

class_t classify(features_t *features, model_t *model) {
  int move_less_error = 0;
  int stat_less_error = 0;
  features_t *model_features;
  int i;

  for (i = 0; i < MODEL_SIZE; ++i) {
    model_features = &model->stationary[i];

    long int stat_mean_err =
        (model_features->meanmag > features->meanmag)
            ? (model_features->meanmag - features->meanmag)
            : (features->meanmag - model_features->meanmag);

    long int stat_sd_err =
        (model_features->stddevmag > features->stddevmag)
            ? (model_features->stddevmag - features->stddevmag)
            : (features->stddevmag - model_features->stddevmag);

    model_features = &model->moving[i];

    long int move_mean_err =
        (model_features->meanmag > features->meanmag)
            ? (model_features->meanmag - features->meanmag)
            : (features->meanmag - model_features->meanmag);

    long int move_sd_err =
        (model_features->stddevmag > features->stddevmag)
            ? (model_features->stddevmag - features->stddevmag)
            : (features->stddevmag - model_features->stddevmag);

    if (move_mean_err < stat_mean_err) {
      move_less_error++;
    } else {
      stat_less_error++;
    }

    if (move_sd_err < stat_sd_err) {
      move_less_error++;
    } else {
      stat_less_error++;
    }
  }

  class_t class =
      move_less_error > stat_less_error ? CLASS_MOVING : CLASS_STATIONARY;
  return class;
}

void record_stats(stats_t *stats, class_t class) {

  stats->totalCount++;

  switch (class) {
  case CLASS_MOVING:

    stats->movingCount++;
    break;

  case CLASS_STATIONARY:

    stats->stationaryCount++;
    break;
  }
}

void warmup_sensor() {
  unsigned int seed = 0;
  unsigned discardedSamplesCount = 0;
  accelReading sample;

  while (discardedSamplesCount++ < NUM_WARMUP_SAMPLES) {
    accel_sample(seed, &sample);
    seed++;
  }
}

void train(features_t *classModel) {
  accelWindow sampleWindow;
  features_t features;
  unsigned i;

  warmup_sensor();

  for (i = 0; i < MODEL_SIZE; ++i) {
    acquire_window(sampleWindow);
    transform(sampleWindow);
    featurize(&features, sampleWindow);

    classModel[i] = features;
  }
}

void recognize(model_t *model) {
  stats_t stats;
  accelWindow sampleWindow;
  features_t features;
  class_t class;
  unsigned i;

  stats.totalCount = 0;
  stats.stationaryCount = 0;
  stats.movingCount = 0;

  for (i = 0; i < SAMPLES_TO_COLLECT; ++i) {
    acquire_window(sampleWindow);
    transform(sampleWindow);
    featurize(&features, sampleWindow);
    class = classify(&features, model);
    record_stats(&stats, class);
  }
}

run_mode_t select_mode(uint8_t *prev_pin_state) {
  uint8_t pin_state = 1;
  count++;
  if (count >= 3)
    pin_state = 2;
  if (count >= 5)
    pin_state = 0;
  if (count >= 7) {
    pin_state = 3;
    count = 0;
#ifdef __MSP430__
#if defined(PORT_CONTROL)
      P3OUT ^= 0x02;
      P3OUT ^= 0x02;
#else
	P1OUT = 0x02;
	_BIC_SR(GIE);
	while(1);
#endif
#endif
  }

  // Don't re-launch training after finishing training
  if ((pin_state == MODE_TRAIN_STATIONARY || pin_state == MODE_TRAIN_MOVING) &&
      pin_state == *prev_pin_state) {
    pin_state = MODE_IDLE;
  } else {
    *prev_pin_state = pin_state;
  }

  return (run_mode_t)pin_state;
}

immortal_thread(ar_main, args) {
  uint8_t prev_pin_state = MODE_IDLE;
  model_t model;

  while (1) {
    run_mode_t mode = select_mode(&prev_pin_state);
    switch (mode) {
    case MODE_TRAIN_STATIONARY:
      train(model.stationary);
      break;
    case MODE_TRAIN_MOVING:
      train(model.moving);
      break;
    case MODE_RECOGNIZE:
      recognize(&model);
      break;
    default:
      break;
    }
  }
}

static immortal_thread_t threads[1];

int main(int argc, char *argv[]) {
#ifdef IMMORTALITY_PORT_LINUX
  im_port_linux_init(argc, argv);
#endif

#ifdef __MSP430__
  WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
#if defined(PORT_CONTROL)
  P3DIR = 0xFF;
#else
  P1OUT = 0x00;
  P1DIR = 0x03;
#endif
#endif

  imt_thread_init(&threads[0], ar_main, NULL);
  imt_run(threads, 1, false);
  return 0;
}

#ifdef PF_SIMULATION

immortalc_fn_ignore void _set_PF_Timer() {

  TA1CCR0 = timer_count;        // max 65535
  TA1CTL = TASSEL__ACLK + MC_1; // set the max period for 16bit timer operation
  TA1CCTL0 = CCIE;              // enable compare reg 0
}

immortalc_fn_ignore static void rtc_init() {
  PJSEL0 = BIT4 | BIT5; // Initialize LFXT pins
  CSCTL0_H = CSKEY_H;   // Unlock CS registers
  CSCTL4 &= ~LFXTOFF;   // Enable LFXT
  do {
    CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
    SFRIFG1 &= ~OFIFG;
  } while (SFRIFG1 & OFIFG); // Test oscillator fault flag
  CSCTL0_H = 0;              // Lock CS registers

  // Setup RTC Timer
  RTCCTL0_H = RTCKEY_H; // Unlock RTC

  RTCCTL0_L = RTCTEVIE_L;                    // RTC event interrupt enable
  RTCCTL13 = RTCSSEL_2 | RTCTEV_0 | RTCHOLD; // Counter Mode, RTC1PS, 8-bit ovf
  RTCPS0CTL = RT0PSDIV1;                     // ACLK, /8
  RTCPS1CTL = RT1SSEL1 | RT1PSDIV0 | RT1PSDIV1; // out from RT0PS, /16

  RTCCTL13 &= ~(RTCHOLD); // Start RTC
}

immortalc_fn_ignore void RebootHandler() {
  rtc_init();
  _set_PF_Timer(); // Power failure simulation
}

/* Timer for Power Failures */
#if defined(__GNUC__)
void __attribute__((interrupt(TIMER1_A0_VECTOR))) _Timer_PF(void)
#else
#error Compiler not supported!
#endif
{
  random_num = RTCPS;
  mod5_rand = random_num % 51;
  random_num = min_pf + mod5_rand * 8;
  timer_count = random_num;
#ifdef PORT_CONTROL
  P3OUT ^= 0x01;
  P3OUT ^= 0x01;
#endif
  PMMCTL0 = 0x0008;
}

#endif
