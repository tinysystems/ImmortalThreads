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
