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
 *  This file is MPS430 processor specific.
 */
#include "immortality.h"

#include "port/portable.h"

/* semaphore that needs to be updated at booting time */
__fram sem_t *volatile _sem_commit = 0;

/* mutex that needs to be updated at booting time */
__fram mutex_t *volatile _mutex_commit = 0;

/* event signal partially committed */
__fram event_t *volatile _event_isr_commit = 0;
/* event wait partially committed */
__fram event_t *volatile _event_commit = 0;

/* to indicate that there was a power failure and this is the recovery */
volatile uint8_t _sched_lock = false;

/* We provide an a mechanism so that the developer can override ImmortalReboot
 */
void Default_Handler(void) __attribute__((weak));
extern void RebootHandler(void) __attribute__((weak, alias("Default_Handler")));

volatile immortal_thread_t *g_imt_priv_current_thread;

void imt_priv_run_thread(immortal_thread_t *thread) {
  g_imt_priv_current_thread = thread;
  thread->fn_ptr(thread->id, thread->args);
}

/* The main scheduler loop. This function is hw dependent.
 *
 * (Do not place any local variable in this function)
 *
 */
void imt_run(immortal_thread_t *volatile threads, uint8_t size, bool timer) {
  /* check any partially committed event */
  if (_event_isr_commit) {
    _event_isr_commit->_signal = 1;
    _event_isr_commit->_idx = _event_isr_commit->__idx;
    _event_isr_commit = 0;
  } else if (_event_commit || _sem_commit ||
             _mutex_commit) { /* check any partially committed event, semaphore
                                 or mutex */
    _sched_lock = true;
    if (_event_commit) {
      /* continue the update of the event */
      imt_priv_run_thread(_event_commit->_fn);
    } else if (_sem_commit) {
      /* continue the update of the semaphore */
      imt_priv_run_thread(_sem_commit->_fn);
    } else {
      /* continue the update of the mutex */
      imt_priv_run_thread(_mutex_commit->_thread);
    }
    _sched_lock = false;
  }

  im_port_init(timer);

  /* call the reboot function, there is a default implementation */
  RebootHandler();

  im_port_run(threads, size);
}

/* This function will be called at each reboot. If needed, override this in the
 * application  */
void Default_Handler(void) {}
