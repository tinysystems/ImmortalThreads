/**
 * \file immortality_conf.h
 * \brief Internal configuration header of ImmortalThreads
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * This configuration header is the one that must be used in the
 * ImmortalThreads codebase.
 *
 * This header checks whether the required macro are defined and for some
 * optional macros it sets a default value.
 *
 * For all the configurable options, please look at the documented options
 * in this file.
 */

#ifndef IMMORTALITY_CONF_H_
#define IMMORTALITY_CONF_H_

#include <stdint.h>

#ifndef IMMORTALITY_MAX_THREAD_COUNT
#define IMMORTALITY_MAX_THREAD_COUNT 1
#endif

#ifndef IMMORTALITY_PRIVATIZATION_BUFFER_COUNT
#define IMMORTALITY_PRIVATIZATION_BUFFER_COUNT 3
#endif

#ifndef IMMORTALITY_PREEMPTION_MS
#define IMMORTALITY_PREEMPTION_MS 100
#endif

#if IMMORTALITY_MAX_THREAD_COUNT <= 8
typedef uint8_t immortal_function_instance_id;
#elif IMMORTALITY_MAX_THREAD_COUNT <= 16
typedef uint16_t immortal_function_instance_id;
#elif IMMORTALITY_MAX_THREAD_COUNT <= 32
typedef uint32_t immortal_function_instance_id;
#elif IMMORTALITY_MAX_THREAD_COUNT <= 64
typedef uint64_t immortal_function_instance_id;
#else
#error "Unsupported number of threads"
#endif

#endif /* ifndef IMMORTALITY_CONF_H_ */
