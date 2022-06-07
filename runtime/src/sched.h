/**
 * \file sched.h
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALITY_SCHED_H_
#define IMMORTALITY_SCHED_H_

#include "common.h"
#include "thread.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * Start the ImmortalThreads scheduler
 *
 * \param [in] threads immortal threads array
 * \param [in] size array size
 * \param [in] timer set periodic scheduler timer (false/true) for round robin
 * scheduling
 */
void imt_run(immortal_thread_t *volatile threads, uint8_t size, bool timer);
void imt_priv_run_thread(immortal_thread_t *thread);

#endif /* IMMORTALITY_SCHED_H_ */
