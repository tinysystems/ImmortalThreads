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
_gdef event_t e_Timer;

_immortal_function(SENSE_TEMP, void *args) {
  _begin(SENSE_TEMP);
  /* sum of the sensed temperature values */
  _def uint32_t sum;
  _WR(sum, 0);
  /* number of temperature samples */
  _def uint8_t cnt;
  _WR(cnt, 0);
  /* local buffer to hold event data */
  _def uint8_t buf[10 /*_EVENT_MAXBYTES*/];

  _def int64_t volatile i;
  /* always init event before using it */
  _EVENT_INIT(e_Timer);

  while (1) {
    _WR(i, 0);
    // EVENT_WAIT_EXPIRES(e_Timer,buf,EXPIRATION);
    _EVENT_WAIT_EXPIRES(e_Timer, buf, 0xffffffff);
    /*EVENT_WAIT*/ /*(e_Timer, buf)*/
    ;
    _WR_SELF(uint8_t, cnt, cnt + 1);
    _WR_SELF(uint32_t, sum, sum + ((uint32_t *)buf)[0]);
    P1OUT ^= (0x0002); /*__led_toggle*/ /*(LED1)*/
    ;
    for (; i < 10000;) {
      ;
      _WR_SELF(volatile int64_t, i, i + 1);
    }
  }
  _end(SENSE_TEMP);
}
