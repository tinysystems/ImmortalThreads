/**
 * \file annotations.h
 * \brief Compiler annotation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#ifndef IMMORTALITY_ANNOTATIONS_H_
#define IMMORTALITY_ANNOTATIONS_H_

#include "immortality_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __clang__
#define immortalc_annotate(str) __attribute__((annotate(str)))
#else
#define immortalc_annotate(str)
#endif

/**
 * This is a hack to ease the implementation of the compiler.
 * The transformation is multipass. All the passes that apply immortal threads
 * runtime macros are grouped as last ones and are performed together.
 * The function copying pass is done before these and it transforms local
 * data access into array access, indexed by `_id`. But `_id` is only
 * introduced by the `_immortal_function` at the later pass...
 *
 * The easiest way is to have this dumb declaration here, so that the
 * transformation can continue without errors to the next pass.
 */
extern immortal_function_instance_id _id;

/**
 * Use this to annotate idempotent functions
 * Idempotent functions are not instrumented
 */
#define immortalc_fn_idempotent immortalc_annotate("immortalc::fn_idempotent")

/**
 * Use this to annotate functions that must not be instrumented
 */
#define immortalc_fn_ignore immortalc_annotate("immortalc::ignore")

/**
 * Use this to annotate the maximum number of instances that a function should
 * have
 */
#define immortalc_fn_max_instances(_n_)                                        \
  immortalc_annotate("immortalc::fn_instances::" #_n_)

/**
 * Use this to tell immortalc to achieve function multiinstance by performing
 * whole function body copying
 */
#define immortalc_fn_whole_body_copy                                           \
  immortalc_annotate("immortalc::fn_whole_body_copy")

/**
 * Private annotation used to annotate immortal threads
 */
#define _immortalc_fn_thread immortalc_annotate("immortalc::thread")

#ifdef __cplusplus
}
#endif

#endif /* ifndef IMMORTALITY_ANNOTATIONS_H_ */
