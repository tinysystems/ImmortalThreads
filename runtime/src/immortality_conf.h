/**
 * \file immortality_config.h
 * \brief Internal configuration header of ImmortalThreads
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 *
 * This configuration header is the one that must be used in the
 * ImmortalThreads codebase.
 *
 * This header checks whether the required macro are defined and for some
 * optional macros it sets a default value.
 *
 * The application developer can define the required macro using the appropriate
 * CLI options of the compiler or by defining only the
 * `IMMORTALITY_INCLUDE_CONFIG_H` macro and then defining the remaining macros
 * in the `immortality_config.h` file.
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
