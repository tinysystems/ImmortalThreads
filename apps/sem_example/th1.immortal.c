/**
 * \file th1.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "../leds.h"
#include "immortality.h"

_gdef extern sem_t thread_2_sem;
_gdef sem_t thread_1_sem;

_immortal_function(thread_1, void *args) {
  _begin(thread_1);
  _def uint32_t i;
  _SEM_INIT(thread_1_sem);

  while (1) {
    _SEM_WAIT(thread_1_sem);
    P1OUT ^= (0x0002); /*__led_toggle*/ /*(LED1)*/
    ;
    {
      _WR(i, 0);
      for (; i < 1000;) {
        ;
        _WR_SELF(uint32_t, i, i + 1);
      }
    }
    _SEM_POST(thread_2_sem);
  }
  _end(thread_1);
}
