/*
 * MIT License
 *
 * Copyright (c) 2020 Eren Yildiz and Kasim Sinan Yildirim
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Adapted from:
 * https://github.com/CMUAbstract/alpaca-oopsla2017/blob/master/src/main_ar_dino.c
 *
 */

#include "immortality.h"
#include <stdio.h>

_gdef unsigned count = 0;

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
typedef accelReading accelWindow[3 /*ACCEL_WINDOW_SIZE*/];

typedef struct {
  unsigned meanmag;
  unsigned stddevmag;
} features_t;

typedef enum {
  CLASS_STATIONARY,
  CLASS_MOVING,
} class_t;

typedef struct {
  features_t stationary[16 /*MODEL_SIZE*/];
  features_t moving[16 /*MODEL_SIZE*/];
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

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ uint32_t
mult16(uint16_t a, uint16_t b) {
  return (uint32_t)a * (uint32_t)b;
}

/* Square root by Newton's method */
__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ uint16_t
sqrt16(uint32_t x) {
  uint16_t hi = 0xffff;
  uint16_t lo = 0;
  uint16_t mid = ((uint32_t)hi + (uint32_t)lo) >> 1;
  uint32_t s = 0;

  while (s != x && hi - lo > 1) {
    mid = ((uint32_t)hi + (uint32_t)lo) >> 1;
    s = mult16(mid, mid);
    if (s < x) {
      lo = mid;
    } else {
      hi = mid;
    }
  }

  return mid;
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ void
accel_sample(unsigned seed, accelReading *result) {
  result->x = (seed * 17) % 85;
  result->y = (seed * 17 * 17) % 85;
  result->z = (seed * 17 * 17 * 17) % 85;
}

_immortal_function(acquire_window, accelWindow window) {
  _begin(acquire_window);
  _def unsigned seed;
  _WR(seed, 1);
  _def accelReading sample;
  _def unsigned samplesInWindow;
  samplesInWindow = 0;

  while (samplesInWindow < 3 /*ACCEL_WINDOW_SIZE*/) {
    accel_sample(seed, &sample);
    _WR_SELF(unsigned int, seed, seed + 1);
    {
      _def unsigned int tmp_var_0;
      tmp_var_0 = samplesInWindow;
      _WR_SELF(unsigned int, samplesInWindow, samplesInWindow + 1);
      window[tmp_var_0] = sample;
    }
  }
  _end(acquire_window);
}

_immortal_function(transform, accelWindow window) {
  _begin(transform);
  _def unsigned i;
  _WR(i, 0);

  {
    i = 0;
    for (; i < 3 /*ACCEL_WINDOW_SIZE*/;) {
      _def accelReading *sample;
      _WR(sample, &window[i]);

      if (sample->x < 10 /*SAMPLE_NOISE_FLOOR*/ ||
          sample->y < 10 /*SAMPLE_NOISE_FLOOR*/ ||
          sample->z < 10 /*SAMPLE_NOISE_FLOOR*/) {

        _WR_SELF(uint16_t, sample->x,
                 (sample->x > 10 /*SAMPLE_NOISE_FLOOR*/) ? sample->x : 0);
        _WR_SELF(uint16_t, sample->y,
                 (sample->y > 10 /*SAMPLE_NOISE_FLOOR*/) ? sample->y : 0);
        _WR_SELF(uint16_t, sample->z,
                 (sample->z > 10 /*SAMPLE_NOISE_FLOOR*/) ? sample->z : 0);
      }
      _WR_SELF(unsigned int, i, i + 1);
    }
  }
  _end(transform);
}

_immortal_function(featurize, features_t *features, accelWindow aWin) {
  _begin(featurize);
  _def accelReading mean;
  _def accelReading stddev;

  _WR(mean.z, 0);
  mean.y = mean.z;
  mean.x = mean.y;
  stddev.z = 0;
  stddev.y = stddev.z;
  stddev.x = stddev.y;
  _def int i;

  {
    i = 0;
    for (; i < 3 /*ACCEL_WINDOW_SIZE*/;) {
      _WR_SELF_CONTEMPORANEOUS_3(uint16_t, mean.x, mean.x + aWin[i].x, uint16_t,
                                 mean.y, mean.y + aWin[i].y, uint16_t, mean.z,
                                 mean.z + aWin[i].z);
      _WR_SELF(int, i, i + 1);
    }
  }

  _WR_SELF_CONTEMPORANEOUS_3(uint16_t, mean.x, mean.x >> 2, uint16_t, mean.y,
                             mean.y >> 2, uint16_t, mean.z, mean.z >> 2);

  {
    i = 0;
    for (; i < 3 /*ACCEL_WINDOW_SIZE*/;) {
      _WR_SELF(uint16_t, stddev.x,
               stddev.x + aWin[i].x > mean.x ? aWin[i].x - mean.x
                                             : mean.x - aWin[i].x); // x
      _WR_SELF(uint16_t, stddev.y,
               stddev.y + aWin[i].y > mean.y ? aWin[i].y - mean.y
                                             : mean.y - aWin[i].y); // y
      _WR_SELF_CONTEMPORANEOUS_2(uint16_t, stddev.z,
                                 stddev.z + aWin[i].z > mean.z
                                     ? aWin[i].z - mean.z
                                     : mean.z - aWin[i].z,
                                 int, i, i + 1);
    }
  }

  _WR_SELF_CONTEMPORANEOUS_3(uint16_t, stddev.x, stddev.x >> 2, uint16_t,
                             stddev.y, stddev.y >> 2, uint16_t, stddev.z,
                             stddev.z >> 2);

  _def unsigned meanmag;
  meanmag = mean.x * mean.x + mean.y * mean.y + mean.z * mean.z;
  _def unsigned stddevmag;
  stddevmag = stddev.x * stddev.x + stddev.y * stddev.y + stddev.z * stddev.z;

  features->meanmag = sqrt16(meanmag);
  features->stddevmag = sqrt16(stddevmag);
  _end(featurize);
}

_immortal_function_with_retval(classify, class_t, features_t *features,
                               model_t *model) {
  _begin(classify);
  _def int move_less_error;
  _WR(move_less_error, 0);
  _def int stat_less_error;
  stat_less_error = 0;
  _def features_t *model_features;
  _def int i;

  {
    i = 0;
    for (; i < 16 /*MODEL_SIZE*/;) {
      _WR(model_features, &model->stationary[i]);

      _def long int stat_mean_err;
      _WR(stat_mean_err, (model_features->meanmag > features->meanmag)
                             ? (model_features->meanmag - features->meanmag)
                             : (features->meanmag - model_features->meanmag));

      _def long int stat_sd_err;
      _WR(stat_sd_err, (model_features->stddevmag > features->stddevmag)
                           ? (model_features->stddevmag - features->stddevmag)
                           : (features->stddevmag - model_features->stddevmag));

      _WR(model_features, &model->moving[i]);

      _def long int move_mean_err;
      _WR(move_mean_err, (model_features->meanmag > features->meanmag)
                             ? (model_features->meanmag - features->meanmag)
                             : (features->meanmag - model_features->meanmag));

      _def long int move_sd_err;
      _WR(move_sd_err, (model_features->stddevmag > features->stddevmag)
                           ? (model_features->stddevmag - features->stddevmag)
                           : (features->stddevmag - model_features->stddevmag));

      if (move_mean_err < stat_mean_err) {
        _WR_SELF(int, move_less_error, move_less_error + 1);
      } else {
        _WR_SELF(int, stat_less_error, stat_less_error + 1);
      }

      if (move_sd_err < stat_sd_err) {
        _WR_SELF(int, move_less_error, move_less_error + 1);
      } else {
        _WR_SELF(int, stat_less_error, stat_less_error + 1);
      }
      _WR_SELF(int, i, i + 1);
    }
  }

  _def class_t class;
  _WR(class,
      move_less_error > stat_less_error ? CLASS_MOVING : CLASS_STATIONARY);
  _return(classify, class);
  _end(classify);
}

_immortal_function(record_stats, stats_t *stats, class_t class) {
  _begin(record_stats);

  _WR_SELF(unsigned int, stats->totalCount, stats->totalCount + 1);

  if (class == CLASS_MOVING) {
    _WR_SELF(unsigned int, stats->movingCount, stats->movingCount + 1);
  } else if (class == CLASS_STATIONARY) {
    _WR_SELF(unsigned int, stats->stationaryCount, stats->stationaryCount + 1);
  }
  _end(record_stats);
}

_immortal_function(warmup_sensor) {
  _begin(warmup_sensor);
  _def unsigned int seed;
  _WR(seed, 0);
  _def unsigned discardedSamplesCount;
  discardedSamplesCount = 0;
  _def accelReading sample;

  {
    _def char condition_tmp;
    {
      _def unsigned int tmp_var_0;
      tmp_var_0 = discardedSamplesCount;
      _WR_SELF(unsigned int, discardedSamplesCount, discardedSamplesCount + 1);
      condition_tmp = (tmp_var_0 < 3);
    }
    while (condition_tmp /*NUM_WARMUP_SAMPLES*/) {
      accel_sample(seed, &sample);
      _WR_SELF(unsigned int, seed, seed + 1);
      {
        _def unsigned int tmp_var_0;
        tmp_var_0 = discardedSamplesCount;
        _WR_SELF(unsigned int, discardedSamplesCount,
                 discardedSamplesCount + 1);
        condition_tmp = tmp_var_0 < 3;
      }
    }
  }
  _end(warmup_sensor);
}

_immortal_function(train, features_t *classModel) {
  _begin(train);
  _def accelWindow sampleWindow;
  _def features_t features;
  _def unsigned i;

  _call(warmup_sensor);

  {
    _WR(i, 0);
    for (; i < 16 /*MODEL_SIZE*/;) {
      _call(acquire_window, sampleWindow);
      _call(transform, sampleWindow);
      _call(featurize, &features, sampleWindow);

      _WR(classModel[i], features);
      _WR_SELF(unsigned int, i, i + 1);
    }
  }
  _end(train);
}

_immortal_function(recognize, model_t *model) {
  _begin(recognize);
  _def stats_t stats;
  _def accelWindow sampleWindow;
  _def features_t features;
  _def class_t class;
  _def unsigned i;

  _WR(stats.totalCount, 0);
  stats.stationaryCount = 0;
  stats.movingCount = 0;

  {
    i = 0;
    for (; i < 128 /*SAMPLES_TO_COLLECT*/;) {
      _call(acquire_window, sampleWindow);
      _call(transform, sampleWindow);
      _call(featurize, &features, sampleWindow);
      _call(classify, &class, &features, model);
      _call(record_stats, &stats, class);
      _WR_SELF(unsigned int, i, i + 1);
    }
  }
  _end(recognize);
}

_immortal_function_with_retval(select_mode, run_mode_t,
                               uint8_t *prev_pin_state) {
  _begin(select_mode);
  _def uint8_t pin_state;
  _WR(pin_state, 1);
  _WR_SELF(unsigned int, count, count + 1);
  if (count >= 3) {
    _WR(pin_state, 2);
  }
  if (count >= 5) {
    _WR(pin_state, 0);
  }
  if (count >= 7) {
    _WR(pin_state, 3);
    count = 0;
  }

  // Don't re-launch training after finishing training
  if ((pin_state == MODE_TRAIN_STATIONARY || pin_state == MODE_TRAIN_MOVING) &&
      pin_state == *prev_pin_state) {
    _WR(pin_state, MODE_IDLE);
  } else {
    _WR(*prev_pin_state, pin_state);
  }

  _return(select_mode, (run_mode_t)pin_state);
  _end(select_mode);
}

_immortal_function(ar_main, void *args) {
  _begin(ar_main);
  _def uint8_t prev_pin_state;
  _WR(prev_pin_state, MODE_IDLE);
  _def model_t model;

  while (1) {
    _def run_mode_t mode;
    _call(select_mode, &mode, &prev_pin_state);
    do {
      if (mode == MODE_TRAIN_STATIONARY) {
        _call(train, model.stationary);
      } else if (mode == MODE_TRAIN_MOVING) {
        _call(train, model.moving);
      } else if (mode == MODE_RECOGNIZE) {
        _call(recognize, &model);
      } else {
        break;
      }
    } while (0);
  }
  _end(ar_main);
}

_gdef static immortal_thread_t threads[1];

int main(int argc, char *argv[]) {
  _imt_thread_init_macro(&threads[0], ar_main, ((void *)0) /*NULL*/);
  imt_run(threads, 1, 0 /*false*/);
  return 0;
}

