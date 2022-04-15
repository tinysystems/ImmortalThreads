/**
 * \file thread.c
 * \brief ImmortalThreads - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#include "thread.h"

#include <assert.h>
#include <stdlib.h>

#if IMMORTALITY_MAX_THREAD_COUNT == 1
__fram privatization_buffer_t _imt_global_privatization_buffer;
#endif

void imt_thread_init(immortal_thread_t *thread, immortal_thread_fn fn,
                     void *args) {}

void _imt_thread_init(immortal_thread_t *thread, _immortal_thread_fn fn,
                      void *args) {
  thread->args = args;
  thread->id = 0;
  thread->fn_ptr = fn;
}

void _imt_thread_init_multi(immortal_thread_t *thread, _immortal_thread_fn fn,
                            void *args,
                            immortal_function_metadata_t *thread_fn_metadata) {
  thread->args = args;

  _di();
  if (thread->fn_ptr != NULL) {
    thread_fn_metadata->instance_bitset &= ~(1u << (thread->id));
    _ei();
    return;
  }
  immortal_function_instance_id inactive =
      __builtin_ffs(thread_fn_metadata->instance_bitset);
  assert(inactive != 0);
  thread->id = inactive - 1;
  thread->fn_ptr = fn;
  thread_fn_metadata->instance_bitset &= ~(1u << (thread->id));
  _ei();
}
