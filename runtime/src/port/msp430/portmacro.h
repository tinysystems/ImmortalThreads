/**
 * \file msp430/port.h
 * \brief MSP430 port - interface
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#ifndef IMMORTALITY_PORT_MSP430_H_
#define IMMORTALITY_PORT_MSP430_H_

#include <msp430.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* for compiler compatibility (FRAM sections) */
#ifdef __GNUC__
#define __fram __attribute__((section(".persistent")))
#define __ro_fram __attribute__((section(".rodata")))
#define __hifram __attribute__((section(".persistent_hifram ")))
#define __ro_hifram __attribute__((section(".upper.rodata")))
#endif

#define _memcpy(dest, src, n) memcpy(dest, src, n)

#define _ei() __enable_interrupt();
#define _di() __disable_interrupt();

#ifdef __cplusplus
}
#endif

#endif /* ifndef IMMORTALITY_PORT_MSP430_H_ */
