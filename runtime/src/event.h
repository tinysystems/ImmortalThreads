/**
 * \file event.h
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * Events are only signalled from ISRs. Event implementation is based on the
 * binary semaphore implementation.
 */

#ifndef IMMORTALITY_EVENT_H_
#define IMMORTALITY_EVENT_H_

#include "thread.h"

#include <stdbool.h>
#include <stdint.h>

#define _EVENT_MAXBYTES 10

typedef struct {
  /**
   * to hold event data
   */
  uint8_t _buffer[2][_EVENT_MAXBYTES];
  /**
   * to hold event _timestamp
   */
  uint32_t _timestamp[2];
  uint8_t volatile _idx;
  /**
   * scratch variable to eliminate WAR dependency
   */
  uint8_t volatile __idx;
  /**
   * signals the events
   */
  uint8_t volatile _signal;
  /**
   * thread that waits on this events
   */
  volatile immortal_thread_t *_fn;
} event_t;

extern event_t *volatile _event_isr_commit;
extern event_t *volatile _event_commit;

/**
 * Initialize an event
 *
 * Shim API for the application developer
 */
void EVENT_INIT(event_t e);
/**
 * Set time stamp of the event
 *
 * Shim API for the application developer
 */
void EVENT_SET_TIMESTAMP(event_t e, uint32_t time);
/**
 * Get time stamp of the event
 *
 * Shim API for the application developer
 */
uint32_t EVENT_GET_TIMESTAMP(event_t e);

/**
 * Get the buffer of the event
 *
 * Shim API for the application developer
 */
uint8_t *EVENT_GET_BUFFER(event_t e);

/**
 * Signal the event
 *
 * Shim API for the application developer
 */
void EVENT_SIGNAL(event_t e);

/**
 * Inside an immortal thread, wait for an event within expiration time.
 *
 * \param [in] expiration_time events not received within expiration_time will
 * be discarded
 */
void EVENT_WAIT_EXPIRES(event_t e, uint8_t *buffer, uint32_t expiration_time);

/**
 * Inside an immortal thread, wait for an event. Without expirtation time.
 */
#define EVENT_WAIT(e, buffer) EVENT_WAIT_EXPIRES(e, buffer, 0xffffffff);

#define _EVENT_INIT(e)                                                         \
  e._idx = 0;                                                                  \
  e._timestamp[0] = 0;                                                         \
  e._signal = 0;                                                               \
  e._fn = g_imt_priv_current_thread;                                           \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__: /* checkpoint */

/* set the timestamp of the event */
#define _EVENT_SET_TIMESTAMP(e, time) e._timestamp[e._idx ^ 1] = time

/* ISR plays with the scratch buffer of the event */
#define _EVENT_GET_BUFFER(e) &e._buffer[e._idx ^ 1][0]

/* this commits the event data and signals the event */
#define _EVENT_SIGNAL(e)                                                       \
  e.__idx = e._idx ^ 1; /* new buffer index */                                 \
  _event_isr_commit =                                                          \
      &e; /* if there is a power failure, we will redo the steps below */      \
  e._signal = 1;    /* upon power failure we can set it again */               \
  e._idx = e.__idx; /* swap buffers */                                         \
  _event_isr_commit = 0;

#define _EVENT_GET_TIMESTAMP(e) e._timestamp[e._idx]

#define _EVENT_WAIT_EXPIRES(e, buffer, EXPIRES)                                \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__: /* checkpoint */                                           \
    _di();          /* disable interrupts */                                   \
    if (e._signal == 0) {                                                      \
      _ei();                                                                   \
      return (void *)0;                                                        \
    }                                                                          \
    /* check data expiration */                                                \
    if ((GETTIME() - e._timestamp[e._idx]) > EXPIRES) {                        \
      e._signal = 0;                                                           \
      _ei();                                                                   \
      return (void *)0;                                                        \
    }                                                                          \
    _event_commit = &e;                                                        \
    e._signal = 0;                                                             \
    /* copy event data to local buffer */                                      \
    _memcpy(buffer, &e._buffer[e._idx][0], _EVENT_MAXBYTES);                   \
    pthis->cur_line = __COUNTER__ + 1;                                         \
  case __COUNTER__: /* checkpoint */                                           \
    _event_commit = 0;                                                         \
    if (_sched_lock == true)                                                   \
      return (void *)0;                                                        \
    _ei(); /* enable interrupts */

/* wait without expiration */
#define _EVENT_WAIT(e, buffer) _EVENT_WAIT_EXPIRES(e, buffer, 0xffffffff);

#endif /* IMMORTALITY_EVENT_H_ */
