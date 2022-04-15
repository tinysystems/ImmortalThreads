/**
 * \file common.h
 * \brief Common utility fucntions and type definitions
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#ifndef IMMORTALITY_COMMON_H_
#define IMMORTALITY_COMMON_H_

#include "immortality_conf.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * privatization buffer for the _WR_SELF macro
   */
  uint64_t volatile buffer;
} privatization_buffer_t;

/**
 * Immortal functions enable continuation and checkpointing
 */
typedef struct immortal_function {
  /**
   * line number that enables local continuation
   */
  volatile uint16_t cur_line;
  /**
   * the instance id of the immortal function that is being called by this
   * immortal function
   */
  volatile immortal_function_instance_id callee_id;
  /**
   * the current function being called by this function
   */
  struct immortal_function *volatile callee;
} immortal_function_t;

typedef struct immortal_function_metadata {
  /**
   * This bitset represents the pool of instances.
   * Bit 0 => instance active
   * Bit 1 => instance inactive
   */
  volatile uint64_t instance_bitset;
} immortal_function_metadata_t;

/**
 * Shim immortal thread type. Visible to the developer.
 */
typedef void (*immortal_thread_fn)(void *args);

/**
 * True immortal thread type. Inserted by the compiler.
 */
typedef immortal_function_t *(*_immortal_thread_fn)(
    immortal_function_instance_id id, void *args);

/**
 * Treat this as opaque type
 */
typedef struct immortal_thread {
  /**
   * self pointer
   */
  _immortal_thread_fn fn_ptr;
  immortal_function_instance_id id;
  void *args;
  privatization_buffer_t privatization_buffer;
} immortal_thread_t;

#ifdef __cplusplus
}
#endif

#endif /* ifndef IMMORTALITY_COMMON_H_ */
