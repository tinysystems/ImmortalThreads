/**
 * \file thread.h
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#ifndef IMMORTALITY_THREAD_H_
#define IMMORTALITY_THREAD_H_

#include "annotations.h"
#include "common.h"
#include "port/portable.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

/* defines the static variables in fram */
#define _def static __fram    /* for the lower FRAM area */
#define __def static __hifram /* for the lower HIFRAM area */

/* global variables */
#define _gdef __fram
#define __gdef __hifram

/**
 * Shim API. Should be used by the developer.
 * The actual API (immortal_function) will be inserted by immortalc.
 */
#define immortal_thread(_fn_name_, _args_)                                     \
  void _fn_name_(void *_args_) _immortalc_fn_thread

/**
 * Define metadata for functions that may have multiple instances.
 * Functions that are guaranteed to be singleton don't need any metadata.
 *
 * FIXME: As for now a top-level immortal function must have metadata defined,
 * regardless of it being a singleton or not.
 */
#define _immortal_function_metadata_def(_static_, _name_, _num_instances_)     \
  _static_ __fram immortal_function_t                                          \
      _name_##_immortal_function_instances[(_num_instances_)];                 \
  _static_ __fram immortal_function_metadata_t                                 \
      _name_##_immortal_function_metadata = {                                  \
          .instance_bitset = ((1u << (_num_instances_)) - 1),                  \
  }

/**
 * Wrap an function declaration with void return type with this macro, to create
 * a proper immortal function declaration.
 */
#define _immortal_function(_name_, ...)                                        \
  immortal_function_t *immortal_##_name_(immortal_function_instance_id _id,    \
                                         ##__VA_ARGS__)

/**
 * Wrap an function declaration with non-void return type with this macro, to
 * create a proper immortal function declaration.
 */
#define _immortal_function_with_retval(_name_, _ret_type_, ...)                \
  immortal_function_t *immortal_##_name_(immortal_function_instance_id _id,    \
                                         _ret_type_ *ret_val, ##__VA_ARGS__)

#define _begin(_name_)                                                         \
  extern volatile immortal_thread_t *g_imt_priv_current_thread;                \
  static __fram immortal_function_t this = {.cur_line = 0};                    \
  immortal_function_t *pthis = &this;                                          \
  switch (pthis->cur_line) {                                                   \
  case 0:

/**
 * Use this instead of _begin for functions that may have multiple instances
 */
#define _begin_multi(_name_, _instance_id_)                                    \
  extern volatile immortal_thread_t *g_imt_priv_current_thread;                \
  immortal_function_t *pthis =                                                 \
      &_name_##_immortal_function_instances[(_instance_id_)];                  \
  switch (pthis->cur_line) {                                                   \
  case 0:

#define _end(_name_)                                                           \
  this.cur_line = __COUNTER__; /* set line counter to a different value */     \
  }                            /*  end switch */                               \
  return &this;                /* return the thread structure to the caller */

#define _end_multi(_name_)                                                     \
  pthis->cur_line = __COUNTER__; /* set line counter to a different value */   \
  }                              /*  end switch */                             \
  return pthis;                  /* return the thread structure to the caller */

/**
 * Shim API called by the developer.
 */
void CHECKPOINT();

#define _CHECKPOINT()                                                          \
  pthis->cur_line = __COUNTER__ + 1;                                           \
  case __COUNTER__:

#define _WR(_arg_, _val_)                                                      \
  _CHECKPOINT();                                                               \
  _arg_ = _val_;

#if IMMORTALITY_MAX_THREAD_COUNT == 1
#define _WR_SELF(_type_, _arg_, _val_)                                         \
  extern privatization_buffer_t _imt_global_privatization_buffer;              \
  _CHECKPOINT();                                                               \
  *((_type_ *)&_imt_global_privatization_buffer.buffer[0]) = _val_;            \
  _CHECKPOINT();                                                               \
  _arg_ = *((_type_ *)&_imt_global_privatization_buffer.buffer[0]);
#define _WR_SELF_CONTEMPORANEOUS_2(_type_, _arg_, _val_, _type1_, _arg1_,      \
                                   _val1_)                                     \
  extern privatization_buffer_t _imt_global_privatization_buffer;              \
  _CHECKPOINT();                                                               \
  *((_type_ *)&_imt_global_privatization_buffer.buffer[0]) = _val_;            \
  *((_type1_ *)&_imt_global_privatization_buffer.buffer[1]) = _val1_;          \
  _CHECKPOINT();                                                               \
  _arg_ = *((_type_ *)&_imt_global_privatization_buffer.buffer[0]);            \
  _arg1_ = *((_type1_ *)&_imt_global_privatization_buffer.buffer[1]);

#define _WR_SELF_CONTEMPORANEOUS_3(_type_, _arg_, _val_, _type1_, _arg1_,      \
                                   _val1_, _type2_, _arg2_, _val2_)            \
  extern privatization_buffer_t _imt_global_privatization_buffer;              \
  _CHECKPOINT();                                                               \
  *((_type_ *)&_imt_global_privatization_buffer.buffer[0]) = _val_;            \
  *((_type1_ *)&_imt_global_privatization_buffer.buffer[1]) = _val1_;          \
  *((_type2_ *)&_imt_global_privatization_buffer.buffer[2]) = _val2_;          \
  _CHECKPOINT();                                                               \
  _arg_ = *((_type_ *)&_imt_global_privatization_buffer.buffer[0]);            \
  _arg1_ = *((_type1_ *)&_imt_global_privatization_buffer.buffer[1]);          \
  _arg2_ = *((_type2_ *)&_imt_global_privatization_buffer.buffer[2]);

#else
#define _WR_SELF(_type_, _arg_, _val_)                                         \
  _CHECKPOINT();                                                               \
  *((_type_ *)&(g_imt_priv_current_thread->privatization_buffer.buffer[0])) =  \
      _val_;                                                                   \
  _CHECKPOINT();                                                               \
  _arg_ = *(                                                                   \
      (_type_ *)&(g_imt_priv_current_thread->privatization_buffer.buffer[0]));
#define _WR_SELF_CONTEMPORANEOUS_2(_type_, _arg_, _val_, _type1_, _arg1_,      \
                                   _val1_)                                     \
  _CHECKPOINT();                                                               \
  *((_type_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[0]) =    \
      _val_;                                                                   \
  *((_type1_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[1]) =   \
      _val1_;                                                                  \
  _CHECKPOINT();                                                               \
  _arg_ =                                                                      \
      *((_type_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[0]); \
  _arg1_ = *(                                                                  \
      (_type1_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[1]);

#define _WR_SELF_CONTEMPORANEOUS_3(_type_, _arg_, _val_, _type1_, _arg1_,      \
                                   _val1_, _type2_, _arg2_, _val2_)            \
  _CHECKPOINT();                                                               \
  *((_type_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[0]) =    \
      _val_;                                                                   \
  *((_type1_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[1]) =   \
      _val1_;                                                                  \
  *((_type2_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[2]) =   \
      _val2_;                                                                  \
  _CHECKPOINT();                                                               \
  _arg_ =                                                                      \
      *((_type_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[0]); \
  _arg1_ = *(                                                                  \
      (_type1_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[1]);  \
  _arg2_ = *(                                                                  \
      (_type2_ *)&g_imt_priv_current_thread->privatization_buffer.buffer[2]);
#endif

/**
 * Call this macro to perform return.
 *
 * \param _name_ name of the current function
 * \param _value_ return value
 *
 * If an immortal function's original signature has a non-void return type, but
 * the return value is not used, then when calling the immortal function
 * ret_val must be a NULL pointer, in which case we don't perform the write and
 * simply return.
 */
#define _return(_name_, _value_)                                               \
  if (ret_val) {                                                               \
    _WR(*ret_val, _value_);                                                    \
  }                                                                            \
  return pthis;

/**
 * Call this macro to perform void return.
 *
 * \param _name_ name of the current function
 * \param _value_ return value
 *
 * Typically used in a function having void return type, in order to perform
 * early return.
 */
#define _return_void(_name_) return &this;

/**
 * Perform checkpoint and enter critical section.  We perform checkpoint before
 * entering the critical section to ensure that only code that is inside the
 * critical section is indeed executed in a critical section.
 *
 * We note that _di() must be idempotent.
 *
 * TODO: for this to work on restart ImmortalThreads must re-disable interrupts
 * and re-execute the last thread that was in a critical section before the
 * last power failure.
 */
#define _ENTER_POWER_FAILURE_SAFE_CRITICAL()                                   \
  _CHECKPOINT();                                                               \
  _di();

/**
 * Perform checkpoint and exit critical section. We perform checkpoint before
 * exiting the critical section to ensure that code that is inside the critical
 * section is indeed executed inside a critical section.
 *
 * Without this checkpoint, if we reach _ei() and re-enable interrupts and then
 * a power failure happens. On thread resume before re-reaching _ei() we may
 * executed some other code outside a critical section, since we already called
 * _ei() in the previous execution. But That code should be executed in a
 * critical section.
 *
 * We note that _ei() must be idempotent.
 */
#define _EXIT_POWER_FAILURE_SAFE_CRITICAL()                                    \
  _CHECKPOINT();                                                               \
  _ei();

/*
 * TODO: take and release should happen atomically.
 * Sort out the interplay between checkpoint, critical section, interrupts,
 * power failures and preemption.
 */
#define _TAKE_IMMORTAL_FUNCTION_INSTANCE(_metadata_, _id_)                     \
  _ENTER_POWER_FAILURE_SAFE_CRITICAL();                                        \
  /* assert at least one instance is available */                              \
  assert(__builtin_ffs((_metadata_)->instance_bitset) != 0);                   \
  (_id_) = __builtin_ffs((_metadata_)->instance_bitset) - 1;                   \
  _CHECKPOINT();                                                               \
  /* Bit toggling is idempotent */                                             \
  (_metadata_)->instance_bitset &= (~(1u << (_id_)));                          \
  _EXIT_POWER_FAILURE_SAFE_CRITICAL();

#define _RELEASE_IMMORTAL_FUNCTION_INSTANCE(_metadata_, _id_)                  \
  _ENTER_POWER_FAILURE_SAFE_CRITICAL();                                        \
  (_metadata_)->instance_bitset |= (1 << (_id_));                              \
  _EXIT_POWER_FAILURE_SAFE_CRITICAL();

/**
 * Shim API called by the developer.
 * Initialize an immortal thread.
 */
void imt_thread_init(immortal_thread_t *thread, immortal_thread_fn fn,
                     void *args);

/**
 * True runtime API of ImmortalThreads.
 * Initialize an immortal thread that has single instance.
 */
void _imt_thread_init(immortal_thread_t *thread, _immortal_thread_fn fn,
                      void *args);

/**
 * True runtime API of ImmortalThreads.
 * Initialize an immortal thread that has multiple instances.
 */
void _imt_thread_init_multi(immortal_thread_t *thread, _immortal_thread_fn fn,
                            void *args,
                            immortal_function_metadata_t *thread_fn_metadata);

#define _imt_thread_init_macro(_thread_, _immortal_fn_name_, _args_)           \
  do {                                                                         \
    extern immortal_function_metadata_t                                        \
        _immortal_fn_name_##_immortal_function_metadata;                       \
    _imt_thread_init(_thread_, (immortal_##_immortal_fn_name_), _args_);       \
  } while (0)

#define _imt_thread_init_multi_macro(_thread_, _immortal_fn_name_, _args_)     \
  do {                                                                         \
    extern immortal_function_metadata_t                                        \
        _immortal_fn_name_##_immortal_function_metadata;                       \
    _imt_thread_init_multi(_thread_, (immortal_##_immortal_fn_name_), _args_,  \
                           &_immortal_fn_name_##_immortal_function_metadata);  \
  } while (0)

#define _call_function(_name_, ...)                                            \
  /* this checkpoint is required to avoid the `pthis->callee->cur_line = 0;`   \
   * line of the previous consecutive call to the same function to cancel the  \
   * progress of the current call */                                           \
  _CHECKPOINT();                                                               \
  pthis->callee = immortal_##_name_(__VA_ARGS__);                              \
  if (!pthis->callee) {                                                        \
    /* If the callee returned NULL, then it is blocked on some concurrency     \
     * primitive. The caller also returns NULL, so that the blocking status    \
     * is propagated */                                                        \
    return NULL;                                                               \
  }                                                                            \
  _CHECKPOINT();                                                               \
  /* reset the line counter. A checkpoint before this line is required because \
   * otherwise if a power failure happens after the assignment, the function   \
   * is re-executed from zero */                                               \
  pthis->callee->cur_line = 0;

/**
 * Wrap calls to immortal functions that are singletons with this macro
 */
#define _call(_name_, ...)                                                     \
  /* pass dumb id: 0 */                                                        \
  _call_function(_name_, 0, ##__VA_ARGS__)

/**
 * Wrap calls to immortal functions that have multiple instances with this macro
 */
#define _call_multi_fn(_name_, ...)                                            \
  extern immortal_function_metadata_t _name_##_immortal_function_metadata;     \
  extern immortal_function_t _name_##_immortal_function_instances[];           \
  _TAKE_IMMORTAL_FUNCTION_INSTANCE(&_name_##_immortal_function_metadata,       \
                                   (pthis->callee_id))                         \
  _call_function(_name_, (pthis->callee_id), ##__VA_ARGS__);                   \
  _RELEASE_IMMORTAL_FUNCTION_INSTANCE(&_name_##_immortal_function_metadata,    \
                                      (pthis->callee_id))

#endif /* IMMORTALITY_THREAD_H_ */
