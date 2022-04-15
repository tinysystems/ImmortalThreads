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
 * if you would like to use GCC-GNU 9.2.0.50 (Mitto System Limited) compiler
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
