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
 * https://raw.githubusercontent.com/CMUAbstract/alpaca-oopsla2017/master/src/main_cuckoo_dino.c
 *
 */

#include "immortality.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//#define PORT_CONTROL

#ifdef PF_SIMULATION
#define min_pf 10
/*__fram int timer_count = 500;*/
/*__fram unsigned int volatile random_num;*/ // read temperature value
                                             /*__fram int mod5_rand;*/
#endif

//#define NUM_BUCKETS 256 // must be a power of 2
#define NUM_BUCKETS 128 // must be a power of 2
//#define NUM_BUCKETS 64 // must be a power of 2
#define MAX_RELOCATIONS 8

typedef uint16_t value_t;
typedef uint16_t hash_t;
typedef uint16_t fingerprint_t;
typedef uint16_t index_t; // bucket index

#define NUM_KEYS (NUM_BUCKETS / 4) // shoot for 25% occupancy
#define INIT_KEY 0x1

__attribute__((annotate(
    "immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ static hash_t
djb_hash(uint8_t *data, unsigned len) {
  uint32_t hash = 5381;
  unsigned int i;

  for (i = 0; i < len; data++, i++) {
    hash = ((hash << 5) + hash) + (*data);
  }

  return hash & 0xFFFF;
}

__attribute__((annotate(
    "immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ static index_t
hash_fp_to_index(fingerprint_t fp) {
  hash_t hash = djb_hash((uint8_t *)&fp, sizeof(fingerprint_t));
  return hash & (128 /*NUM_BUCKETS*/ - 1); // NUM_BUCKETS must be power of 2
}

__attribute__((annotate(
    "immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ static index_t
hash_key_to_index(value_t fp) {
  hash_t hash = djb_hash((uint8_t *)&fp, sizeof(value_t));
  return hash & (128 /*NUM_BUCKETS*/ - 1); // NUM_BUCKETS must be power of 2
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/
static fingerprint_t
hash_to_fingerprint(value_t key) {
  return djb_hash((uint8_t *)&key, sizeof(value_t));
}

__attribute__((annotate(
    "immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ static value_t
generate_key(value_t prev_key) {
  // insert pseufo-random integers, for testing
  // If we use consecutive ints, they hash to consecutive DJB hashes...
  // NOTE: we are not using rand(), to have the sequence available to verify
  // that that are no false negatives (and avoid having to save the values).
  return (prev_key + 1) * 17;
}

_immortal_function_with_retval(insert, _Bool, fingerprint_t *filter,
                               value_t key) {
  _begin(insert);
  _def fingerprint_t fp1, fp2, fp_victim, fp_next_victim;
  _def index_t index_victim, fp_hash_victim;
  _def unsigned relocation_count;
  _WR(relocation_count, 0);

  _def fingerprint_t fp;
  _WR(fp, hash_to_fingerprint(key));

  _def index_t index1;
  _WR(index1, hash_key_to_index(key));

  _def index_t fp_hash;
  _WR(fp_hash, hash_fp_to_index(fp));
  _def index_t index2;
  _WR(index2, index1 ^ fp_hash);

  _WR(fp1, filter[index1]);

  if (!fp1) { // slot 1 is free
    _WR(filter[index1], fp);
  } else {
    _WR(fp2, filter[index2]);
    if (!fp2) { // slot 2 is free
      _WR(filter[index2], fp);
    } else {               // both slots occupied, evict
      if (rand() & 0x80) { // don't use lsb because it's systematic
        _WR(index_victim, index1);
        _WR(fp_victim, fp1);
      } else {
        _WR(index_victim, index2);
        _WR(fp_victim, fp2);
      }

      _WR(filter[index_victim], fp); // evict victim

      {
        _def char condition_tmp;
        do { // relocate victim(s)
          _WR(fp_hash_victim, hash_fp_to_index(fp_victim));
          _WR_SELF(index_t, index_victim, index_victim ^ fp_hash_victim);

          _WR(fp_next_victim, filter[index_victim]);
          _WR(filter[index_victim], fp_victim);

          _WR(fp_victim, fp_next_victim);
          _WR_SELF(unsigned int, relocation_count, relocation_count + 1);
          _WR(condition_tmp, fp_victim && relocation_count < 8);
        } while (condition_tmp /*MAX_RELOCATIONS*/);
      }

      if (fp_victim) {
        _return(insert, 0) /*false*/;
      }
    }
  }

  _return(insert, 1) /*true*/;
  _end(insert);
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/
static _Bool                               /*bool*/
lookup(fingerprint_t *filter, value_t key) {
  fingerprint_t fp = hash_to_fingerprint(key);
  index_t index1 = hash_key_to_index(key);

  index_t fp_hash = hash_fp_to_index(fp);
  index_t index2 = index1 ^ fp_hash;
  return filter[index1] == fp || filter[index2] == fp;
}

_immortal_function(cuckoo_main, void *args) {
  _begin(cuckoo_main);
  _def unsigned i;
  _def value_t key;
  _def fingerprint_t filter[128 /*NUM_BUCKETS*/];

  while (1) {
    {
      _WR(i, 0);
      for (; i < 128 /*NUM_BUCKETS*/;) {
        _WR(filter[i], 0);
        _WR_SELF(unsigned int, i, i + 1);
      }
    }

    _WR(key, 0x1) /*INIT_KEY*/;
    _def unsigned inserts;
    _WR(inserts, 0);
    {
      _WR(i, 0);
      for (; i < (128 / 4) /*NUM_KEYS*/;) {
        _WR_SELF(value_t, key, generate_key(key));
        _def _Bool /*bool*/ success;
        _call(insert, &success, filter, key);
        _WR_SELF(unsigned int, inserts, inserts + success);
        _WR_SELF(unsigned int, i, i + 1);
      }
    }

    _WR(key, 0x1) /*INIT_KEY*/;
    _def unsigned members;
    _WR(members, 0);
    {
      _WR(i, 0);
      for (; i < (128 / 4) /*NUM_KEYS*/;) {
        _WR_SELF(value_t, key, generate_key(key));
        _def _Bool /*bool*/ member;
        _WR(member, lookup(filter, key));
        if (!member) {
          _def fingerprint_t fp;
          _WR(fp, hash_to_fingerprint(key));
        }
        _WR_SELF(unsigned int, members, members + member);
        _WR_SELF(unsigned int, i, i + 1);
      }
    }

#ifdef IMMORTALITY_PORT_LINUX
    /*printf("members: %u, key: %u, inserts: %u\n", members, key, inserts);*/
#endif

    if (members == 32 && key == 58145 && inserts == 32) {
#ifdef __MSP430__
#if defined(PORT_CONTROL)
      /*P3OUT ^= 0x02;*/
      /*P3OUT ^= 0x02;*/
#else
      P1OUT = 0x02;
      __asm__ __volatile__(
          "bic.w %0, SR { nop"
          :
          : "ri"((unsigned int)(0x0008))) /*_BIC_SR*/ /*(GIE)*/;
      while (1) {
        ;
      }
#endif
#endif
    } else {
#ifdef __MSP430__
#if defined(PORT_CONTROL)
      /*P3OUT ^= 0x02;*/
      /*P3OUT ^= 0x02;*/
#else
      P1OUT = 0x01;
      __asm__ __volatile__(
          "bic.w %0, SR { nop"
          :
          : "ri"((unsigned int)(0x0008))) /*_BIC_SR*/ /*(GIE)*/;
      while (1) {
        ;
      }
#endif
#endif
    }
  }
  _end(cuckoo_main);
}

_gdef static immortal_thread_t threads[1];

int main(int argc, char *argv[]) {
#ifdef IMMORTALITY_PORT_LINUX
  /*im_port_linux_init(argc, argv);*/
#endif

#ifdef __MSP430__
  WDTCTL = (0x5a00) /*WDTPW*/ | (0x0080) /*WDTHOLD*/; // stop watchdog timer
  PM5CTL0 &= ~(0x0001) /*LOCKLPM5*/; // Disable the GPIO power-on default
                                     // high-impedance mode
#if defined(PORT_CONTROL)
  /*P3DIR = 0xFF;*/
#else
  P1OUT = 0x00;
  P1DIR = 0x03;
#endif
#endif

  _imt_thread_init_macro(&threads[0], cuckoo_main, ((void *)0) /*NULL*/);
  imt_run(threads, 1, 0 /*false*/);
  return 0;
}

#ifdef PF_SIMULATION
/*immortalc_fn_ignore void _set_PF_Timer() {*/

/*TA1CCR0 = timer_count;*/        // max 65535
/*TA1CTL = TASSEL__ACLK + MC_1;*/ // set the max period for 16bit timer
                                  // operation
/*TA1CCTL0 = CCIE;*/              // enable compare reg 0
                                  /*}*/

/*immortalc_fn_ignore static void rtc_init() {*/
/*PJSEL0 = BIT4 | BIT5;*/ // Initialize LFXT pins
/*CSCTL0_H = CSKEY_H;*/   // Unlock CS registers
/*CSCTL4 &= ~LFXTOFF;*/   // Enable LFXT
/*do {*/
/*CSCTL5 &= ~LFXTOFFG;*/       // Clear LFXT fault flag
                               /*SFRIFG1 &= ~OFIFG;*/
/*} while (SFRIFG1 & OFIFG);*/ // Test oscillator fault flag
/*CSCTL0_H = 0;*/              // Lock CS registers

// Setup RTC Timer
/*RTCCTL0_H = RTCKEY_H;*/ // Unlock RTC

/*RTCCTL0_L = RTCTEVIE_L;*/                       // RTC event interrupt enable
/*RTCCTL13 = RTCSSEL_2 | RTCTEV_0 | RTCHOLD;*/    // Counter Mode, RTC1PS, 8-bit
                                                  // ovf
/*RTCPS0CTL = RT0PSDIV1;*/                        // ACLK, /8
/*RTCPS1CTL = RT1SSEL1 | RT1PSDIV0 | RT1PSDIV1;*/ // out from RT0PS, /16

/*RTCCTL13 &= ~(RTCHOLD);*/ // Start RTC
                            /*}*/

/*immortalc_fn_ignore void RebootHandler() {*/
/*rtc_init();*/
/*_set_PF_Timer();*/ // Power failure simulation
                     /*}*/

/* Timer for Power Failures */
#if defined(__GNUC__)
/*void __attribute__((interrupt(TIMER1_A0_VECTOR))) _Timer_PF(void)*/
#else
#error Compiler not supported!
#endif
/*{*/
/*random_num = RTCPS;*/
/*mod5_rand = random_num % 51;*/
/*random_num = min_pf + mod5_rand * 8;*/
/*timer_count = random_num;*/
#ifdef PORT_CONTROL
/*P3OUT ^= 0x01;*/
/*P3OUT ^= 0x01;*/
#endif
/*PMMCTL0 = 0x0008;*/
/*}*/
#endif
