/**
 * \file immortality.h
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * If you would like to use GCC-GNU 9.2.0.50 (Mitto System Limited) compiler
 * version, you need to use  * msp430fr5994.ld file which is in the
 * msp430fr5994_linker_script_file folder. That compiler version has a bug about
 * TIMER ISR.
 */

#ifndef IMMORTALITY_H_
#define IMMORTALITY_H_

#include <stdint.h>

#ifdef IMMORTALITY_PORT_SPECIAL_AVAILABLE
#include "port_special.h"
#endif
#include "annotations.h"
#include "event.h"
#include "mutex.h"
#include "sched.h"
#include "sem.h"
#include "thread.h"
#include "time.h"

#endif /* IMMORTALITY_H_ */
