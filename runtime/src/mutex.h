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
 *  TODO: Mutex ownership needs to be implemented. As of now, anybody can call
 * _LEAVE...
 */

#ifndef IMMORTALITY_MUTEX_H_
#define IMMORTALITY_MUTEX_H_

#include "thread.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t volatile _lock;
  volatile immortal_thread_t *_thread;
} mutex_t;

/* mutex that needs to be updated at booting time */
extern mutex_t *volatile _mutex_commit;

/* to see if there is a power failure and the scheduler is committing a partial
 * update */
extern volatile uint8_t _sched_lock;

/**
 * Initialize a mutex
 *
 * Shim API for the application developer
 */
void LOCK_INIT(mutex_t lock);

/**
 * Lock a mutex
 *
 * Shim API for the application developer
 */
void ENTER(mutex_t lock);

/**
 * Unlock a mutex
 *
 * Shim API for the application developer
 */
void LEAVE(mutex_t lock);

#define _LOCK_INIT(m)                                                          \
  extern volatile immortal_thread_t *g_imt_priv_current_thread;                \
  m._lock = 0;                                                                 \
  m._thread = g_imt_priv_current_thread;                                       \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__: /* checkpoint */

#define _ENTER(m)                                                              \
  _di();                                                                       \
  if (m._lock == 1) {                                                          \
    _ei();                                                                     \
    return (void *)0;                                                          \
  }                                                                            \
  _mutex_commit = &m;                                                          \
  m._lock = 1;                                                                 \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__: /* checkpoint */                                           \
    _mutex_commit = 0;                                                         \
    /* check if called at start-up due to power failurer */                    \
    if (_sched_lock == true)                                                   \
      return (void *)0;                                                        \
    _ei(); /* enable interrupts */

/* TODO: implement a mutex ownership mechanism */
#define _LEAVE(m)                                                              \
  _di();                                                                       \
  _mutex_commit = &m;                                                          \
  m._lock = 0;                                                                 \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__: /* checkpoint */                                           \
    _mutex_commit = 0;                                                         \
    /* check if called at start-up due to power failurer */                    \
    if (_sched_lock == true)                                                   \
      return (void *)0;                                                        \
    _ei(); /* enable interrupts */

#endif /* IMMORTALITY_MUTEX_H_ */
