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

_immortal_function(SENSE_TEMP,
                   void *args) /*immortal_thread*/ /*(SENSE_TEMP, args)*/;

/* define the threads in order for the RR scheduler */
_gdef immortal_thread_t threads[1 /*NUM_THREADS*/];

int main(void) {
  // stop watchdog timer
  WDTCTL = (0x5a00) /*WDTPW*/ | (0x0080) /*WDTHOLD*/;
  // Disable the GPIO power-on default high-impedance mode
  PM5CTL0 &= ~(0x0001) /*LOCKLPM5*/;

  P1OUT &= ~(0x0002);
  P1DIR |= (0x0002);
  P1OUT &= ~(0x0002); /*__led_init*/ /*(LED1)*/
  ;
  //__led_init(LED2);
  P1OUT |= (0x0002); /*__led_on*/ /*(LED1)*/
  ;
  //__led_on(LED2);

  _imt_thread_init_macro(&threads[0], SENSE_TEMP, ((void *)0) /*NULL*/);
  imt_run(threads, 1 /*NUM_THREADS*/, 1 /*true*/);
  return 0;
}
