/**
 * \file portable.h
 * \brief Porting layer - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * This file contains functions that every port shall implement.
 */
#ifndef IMMORTALITY_PORT_H_
#define IMMORTALITY_PORT_H_

#include "../common.h"

#include "portmacro.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Platform-specific initialization.
 *
 * \param [in] use_timer if true, a periodic context switching should be
 * performed.
 */
void im_port_init(bool use_timer);
/**
 * Platform-specific scheduler implementation
 */
void im_port_run(immortal_thread_t *volatile threads, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /* ifndef IMMORTALITY_PORT_H_ */
