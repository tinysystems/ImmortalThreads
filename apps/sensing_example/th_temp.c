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
