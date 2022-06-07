/**
 * \file timer_event.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
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
