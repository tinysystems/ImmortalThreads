/**
 * \file mem.h
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * NOTES: If you use msp430fr5994, you can use "msp430fr5994.ld" file which is
 * put in the msp430fr5994_linker_script_file folder. If you have another
 * microcontroller you need to configure your linker script file for sections
 * that are stated above.
 */

#ifndef IMMORTALITY_MEM_H_
#define IMMORTALITY_MEM_H_

/* for compiler compatibility (FRAM sections) */
#ifdef __GNUC__
#define __fram __attribute__((section(".persistent")))
#define __ro_fram __attribute__((section(".rodata")))
#define __hifram __attribute__((section(".persistent_hifram ")))
#define __ro_hifram __attribute__((section(".upper.rodata")))
#endif

#include <string.h>
#define _memcpy(dest, src, n) memcpy(dest, src, n)

#endif /* IMMORTALITY_MEM_H_ */
