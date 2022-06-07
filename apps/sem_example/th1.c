/**
 * \file th1.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "../leds.h"
#include "immortality.h"

extern sem_t thread_2_sem;
sem_t thread_1_sem;

immortal_thread(thread_1, args) {
  uint32_t i;
  SEM_INIT(thread_1_sem);

  while (1) {
    SEM_WAIT(thread_1_sem);
    __led_toggle(LED1);
    for (i = 0; i < 1000; i++)
      ;
    SEM_POST(thread_2_sem);
  }
}
