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
 */

#include "../leds.h"
#include "immortality.h"

_gdef extern sem_t thread_2_sem;
_gdef sem_t thread_1_sem;

_immortal_function(thread_1, void *args) {
  _begin(thread_1);
  _def uint32_t i;
  _SEM_INIT(thread_1_sem);

  while (1) {
    _SEM_WAIT(thread_1_sem);
    P1OUT ^= (0x0002); /*__led_toggle*/ /*(LED1)*/
    ;
    {
      _WR(i, 0);
      for (; i < 1000;) {
        ;
        _WR_SELF(uint32_t, i, i + 1);
      }
    }
    _SEM_POST(thread_2_sem);
  }
  _end(thread_1);
}
