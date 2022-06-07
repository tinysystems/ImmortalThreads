/**
 * \file main.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "../leds.h"
#include "immortality.h"
#include "thread.h"

#include <msp430.h>

#define NUM_THREADS 1

immortal_thread(SENSE_TEMP, args);

/* define the threads in order for the RR scheduler */
immortal_thread_t threads[NUM_THREADS];

int main(void) {
  // stop watchdog timer
  WDTCTL = WDTPW | WDTHOLD;
  // Disable the GPIO power-on default high-impedance mode
  PM5CTL0 &= ~LOCKLPM5;

  __led_init(LED1);
  //__led_init(LED2);
  __led_on(LED1);
  //__led_on(LED2);

  imt_thread_init(&threads[0], SENSE_TEMP, NULL);
  imt_run(threads, NUM_THREADS, true);
  return 0;
}
