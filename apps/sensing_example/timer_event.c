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

#include "immortality.h"

#define NUM_TICKS 200

extern event_t e_Timer;

/* the periodic timer */
static void _setPeriodicTimer() {
  TB0CCR0 = NUM_TICKS;
  TB0CTL = TASSEL__ACLK + MC__UP;
  TB0CCTL0 = CCIE;
}

/**
 * This is called each time the device reboots upon a power failure.
 * If not provided, the default implementation is used (in sched.c)
 */
void RebootHandler(void) {
  /* set the timer again */
  _setPeriodicTimer();
}

/* produce random values for the sensed temperature */
uint32_t readTemp() { return rand(); }

/* Timer to generate thread switch */
#if defined(__GNUC__)
void __attribute__((interrupt(TIMER0_B0_VECTOR))) _TimerISR(void)
#else
#error Compiler not supported!
#endif
{
  /* get data buffer of the event */
  uint32_t *buffer = (uint32_t *)_EVENT_GET_BUFFER(e_Timer);
  /* read temperature and put it into the event data */
  *buffer = readTemp();
  /* set event timestamp */
  _EVENT_SET_TIMESTAMP(e_Timer, GETTIME());
  /* notify the thread waiting on the event */
  _EVENT_SIGNAL(e_Timer);
}
