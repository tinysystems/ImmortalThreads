/**
 * \file th2.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "../leds.h"
#include "immortality.h"

sem_t thread_2_sem;
extern sem_t thread_1_sem;

immortal_thread(thread_2, args) {
  uint32_t i;
  SEM_INIT(thread_2_sem);

  while (1) {
    SEM_POST(thread_1_sem);
    SEM_WAIT(thread_2_sem);
    __led_toggle(LED2);
    for (i = 0; i < 1000; i++)
      ;
  }
}
