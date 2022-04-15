/*
 * MIT License
 *
 * Copyright (c) 2020 Eren Yildiz and Kasim Sinan Yildirim
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
