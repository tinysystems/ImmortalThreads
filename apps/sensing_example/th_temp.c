/**
 * \file th_temp.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "../leds.h"
#include "immortality.h"

#define EXPIRATION 1000 /* the expiration time of the event */

/* Timer event (global variables are already in FRAM) */
event_t e_Timer;

immortal_thread(SENSE_TEMP, args) {
  /* sum of the sensed temperature values */
  uint32_t sum = 0;
  /* number of temperature samples */
  uint8_t cnt = 0;
  /* local buffer to hold event data */
  uint8_t buf[_EVENT_MAXBYTES];

  int64_t volatile i;
  /* always init event before using it */
  EVENT_INIT(e_Timer);

  while (1) {
    i = 0;
    // EVENT_WAIT_EXPIRES(e_Timer,buf,EXPIRATION);
    EVENT_WAIT(e_Timer, buf);
    ++cnt;
    sum += ((uint32_t *)buf)[0];
    __led_toggle(LED1);
    for (; i < 10000; i++)
      ;
  }
}
