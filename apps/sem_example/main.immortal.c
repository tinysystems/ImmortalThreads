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

_immortal_function(thread_1,
                   void *args) /*immortal_thread*/ /*(thread_1, args)*/;
_immortal_function(thread_2,
                   void *args) /*immortal_thread*/ /*(thread_2, args)*/;

/* define the threads in order for the RR scheduler */
_gdef immortal_thread_t threads[2 /*NUM_THREADS*/];

int main(void) {
  // stop watchdog timer
  WDTCTL = (0x5a00) /*WDTPW*/ | (0x0080) /*WDTHOLD*/;
  // Disable the GPIO power-on default high-impedance mode
  PM5CTL0 &= ~(0x0001) /*LOCKLPM5*/;

  P1OUT &= ~(0x0002);
  P1DIR |= (0x0002);
  P1OUT &= ~(0x0002); /*__led_init*/ /*(LED1)*/
  ;
  P1OUT &= ~(0x0001);
  P1DIR |= (0x0001);
  P1OUT &= ~(0x0001); /*__led_init*/ /*(LED2)*/
  ;
  P1OUT &= ~(0x0002); /*__led_off*/ /*(LED1)*/
  ;
  P1OUT &= ~(0x0001); /*__led_off*/ /*(LED2)*/
  ;

  _imt_thread_init_macro(&threads[0], thread_1, ((void *)0) /*NULL*/);
  _imt_thread_init_macro(&threads[1], thread_2, ((void *)0) /*NULL*/);
  imt_run(threads, 2 /*NUM_THREADS*/, 1 /*true*/);

  return 0;
}
