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
 * Binary Semaphore Implementation
 */

#ifndef IMMORTALITY_SEM_H_
#define IMMORTALITY_SEM_H_

#include "thread.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t volatile _val;
  volatile immortal_thread_t *_fn;
} sem_t;

/**
 * semaphore that needs to be updated at booting timestamp
 */
extern sem_t *volatile _sem_commit;
/**
 * to see if there is a power failure and the scheduler is committing a partial
 * updated
 */
extern volatile uint8_t _sched_lock;

/**
 * Initialize a binary semaphore
 *
 * Shim API for the application developer
 */
void SEM_INIT(sem_t sem);

/**
 * Wait on a semaphore.
 *
 * Shim API for the application developer
 */
void SEM_WAIT(sem_t sem);

/**
 * Post semaphore from ISR
 *
 * Shim API for the application developer
 */
void SEM_POST_ISR(sem_t sem);

/**
 * Posts semaphore from an immortal thread.
 *
 * Shim API for the application developer
 */
void SEM_POST(sem_t sem);

#define _SEM_INIT(sem)                                                         \
  extern volatile immortal_thread_t *g_imt_priv_current_thread;                \
  sem._val = 0;                                                                \
  sem._fn = g_imt_priv_current_thread;                                         \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__: /* checkpoint */

/* Wait on a semaphore. This code is executed ATOMICALLY. */
#define _SEM_WAIT(sem)                                                         \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__: /* checkpoint */                                           \
    _di();          /* disable interrupts */                                   \
    if (sem._val == 0) {                                                       \
      _ei();                                                                   \
      return (void *)0;                                                        \
    }                                                                          \
    _sem_commit = &sem;                                                        \
    sem._val = 0;                                                              \
    pthis->cur_line = __COUNTER__ + 1;                                         \
  case __COUNTER__: /* checkpoint */                                           \
    _sem_commit = 0;                                                           \
    if (_sched_lock == true)                                                   \
      return (void *)0;                                                        \
    _ei(); /* enable interrupts */

/* posts semaphore from ISR and records the timestamp */
#define _SEM_POST_ISR(sem) sem._val = 1;

/* Posts semaphore from a foreground execution. This code is executed
 * ATOMICALLY. */
#define _SEM_POST(sem)                                                         \
  _di(); /* disable interrupts */                                              \
  _sem_commit = &sem;                                                          \
  sem._val = 1;                                                                \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__: /* checkpoint */                                           \
    _sem_commit = 0;                                                           \
    /* check if called at start-up due to power failurer */                    \
    if (_sched_lock == true)                                                   \
      return (void *)0;                                                        \
    _ei(); /* enable interrupts */

#endif /* IMMORTALITY_SEM_H_ */
