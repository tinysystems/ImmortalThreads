/**
 * \file main.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "../leds.h"
#include "immortality.h"
#include <msp430.h>

#define NUM_THREADS 2

immortal_thread(thread_1, args);
immortal_thread(thread_2, args);

/* define the threads in order for the RR scheduler */
immortal_thread_t threads[NUM_THREADS];

int main(void) {
  // stop watchdog timer
  WDTCTL = WDTPW | WDTHOLD;
  // Disable the GPIO power-on default high-impedance mode
  PM5CTL0 &= ~LOCKLPM5;

  __led_init(LED1);
  __led_init(LED2);
  __led_off(LED1);
  __led_off(LED2);

  imt_thread_init(&threads[0], thread_1, NULL);
  imt_thread_init(&threads[1], thread_2, NULL);
  imt_run(threads, NUM_THREADS, true);

  return 0;
}
