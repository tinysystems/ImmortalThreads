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
 * Adapted from:
 * https://github.com/CMUAbstract/alpaca-oopsla2017/blob/master/src/main_bc_dino.c
 *
 */

#include "immortality.h"
#include <stdio.h>

#define SEED 4L
#define ITER 100
#define CHAR_BIT 8

_gdef static char bits[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, /* 0   - 15  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, /* 16  - 31  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, /* 32  - 47  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 48  - 63  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, /* 64  - 79  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 80  - 95  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 96  - 111 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, /* 112 - 127 */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, /* 128 - 143 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 144 - 159 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 160 - 175 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, /* 176 - 191 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, /* 192 - 207 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, /* 208 - 223 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, /* 224 - 239 */
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8  /* 240 - 255 */
};

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ int
btbl_bitcnt(uint32_t x) {
  int cnt = bits[((char *)&x)[0] & 0xFF];

  if (0L != (x >>= 8)) {
    cnt += btbl_bitcnt(x);
  }
  return cnt;
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ int
bit_count(uint32_t x) {
  int n = 0;

  if (x) {
    do {
      n++;
    } while (0 != (x = x & (x - 1)));
  }
  return (n);
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ int
bitcount(uint32_t i) {
  i = ((i & 0xAAAAAAAAL) >> 1) + (i & 0x55555555L);
  i = ((i & 0xCCCCCCCCL) >> 2) + (i & 0x33333333L);
  i = ((i & 0xF0F0F0F0L) >> 4) + (i & 0x0F0F0F0FL);
  i = ((i & 0xFF00FF00L) >> 8) + (i & 0x00FF00FFL);
  i = ((i & 0xFFFF0000L) >> 16) + (i & 0x0000FFFFL);
  return (int)i;
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ int
ntbl_bitcount(uint32_t x) {
  return bits[(int)(x & 0x0000000FUL)] + bits[(int)((x & 0x000000F0UL) >> 4)] +
         bits[(int)((x & 0x00000F00UL) >> 8)] +
         bits[(int)((x & 0x0000F000UL) >> 12)] +
         bits[(int)((x & 0x000F0000UL) >> 16)] +
         bits[(int)((x & 0x00F00000UL) >> 20)] +
         bits[(int)((x & 0x0F000000UL) >> 24)] +
         bits[(int)((x & 0xF0000000UL) >> 28)];
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ int
BW_btbl_bitcount(uint32_t x) {
  union {
    unsigned char ch[4];
    long y;
  } U;

  U.y = x;

  return bits[U.ch[0]] + bits[U.ch[1]] + bits[U.ch[3]] + bits[U.ch[2]];
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ int
AR_btbl_bitcount(uint32_t x) {
  unsigned char *Ptr = (unsigned char *)&x;
  int Accu;

  Accu = bits[*Ptr++];
  Accu += bits[*Ptr++];
  Accu += bits[*Ptr++];
  Accu += bits[*Ptr];

  return Accu;
}

__attribute__((
    annotate("immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ int
ntbl_bitcnt(uint32_t x) {
  int cnt = bits[(int)(x & 0x0000000FL)];

  if (0L != (x >>= 4)) {
    cnt += ntbl_bitcnt(x);
  }

  return cnt;
}

__attribute__((annotate(
    "immortalc::fn_idempotent"))) /*immortalc_fn_idempotent*/ static int
bit_shifter(uint32_t x) {
  int i, n;
  for (i = n = 0; x && (i < (sizeof(uint32_t) * 8 /*CHAR_BIT*/));
       ++i, x >>= 1) {
    n += (int)(x & 1L);
  }
  return n;
}

_immortal_function(bc_main, void *args) {
  _begin(bc_main);
  _def unsigned n_0, n_1, n_2, n_3, n_4, n_5, n_6;
  _def uint32_t seed;
  _def unsigned iter;
  _def unsigned func;

  while (1) {
    _WR(n_0, 0);
    _WR(n_1, 0);
    _WR(n_2, 0);
    _WR(n_3, 0);
    _WR(n_4, 0);
    _WR(n_5, 0);
    _WR(n_6, 0);

    {
      _WR(func, 0);
      for (; func < 7;) {

        _WR(seed, (uint32_t)4L) /*SEED*/;
        if (func == 0) {
          {
            _WR(iter, 0);
            for (; iter < 100 /*ITER*/;) {
              _WR_SELF(unsigned int, n_0, n_0 + bit_count(seed));
              _WR_SELF(unsigned int, iter, iter + 1);
              _WR_SELF(uint32_t, seed, seed + 13);
              iter, seed;
            }
          }
        } else if (func == 1) {
          {
            _WR(iter, 0);
            for (; iter < 100 /*ITER*/;) {
              _WR_SELF(unsigned int, n_1, n_1 + bitcount(seed));
              _WR_SELF(unsigned int, iter, iter + 1);
              _WR_SELF(uint32_t, seed, seed + 13);
              iter, seed;
            }
          }
        } else if (func == 2) {
          {
            _WR(iter, 0);
            for (; iter < 100 /*ITER*/;) {
              _WR_SELF(unsigned int, n_2, n_2 + ntbl_bitcnt(seed));
              _WR_SELF(unsigned int, iter, iter + 1);
              _WR_SELF(uint32_t, seed, seed + 13);
              iter, seed;
            }
          }
        } else if (func == 3) {
          {
            _WR(iter, 0);
            for (; iter < 100 /*ITER*/;) {
              _WR_SELF(unsigned int, n_3, n_3 + ntbl_bitcount(seed));
              _WR_SELF(unsigned int, iter, iter + 1);
              _WR_SELF(uint32_t, seed, seed + 13);
              iter, seed;
            }
          }
        } else if (func == 4) {
          {
            _WR(iter, 0);
            for (; iter < 100 /*ITER*/;) {
              _WR_SELF(unsigned int, n_4, n_4 + BW_btbl_bitcount(seed));
              _WR_SELF(unsigned int, iter, iter + 1);
              _WR_SELF(uint32_t, seed, seed + 13);
              iter, seed;
            }
          }
        } else if (func == 5) {
          {
            _WR(iter, 0);
            for (; iter < 100 /*ITER*/;) {
              _WR_SELF(unsigned int, n_5, n_5 + AR_btbl_bitcount(seed));
              _WR_SELF(unsigned int, iter, iter + 1);
              _WR_SELF(uint32_t, seed, seed + 13);
              iter, seed;
            }
          }
        } else if (func == 6) {
          {
            _WR(iter, 0);
            for (; iter < 100 /*ITER*/;) {
              _WR_SELF(unsigned int, n_6, n_6 + bit_shifter(seed));
              _WR_SELF(unsigned int, iter, iter + 1);
              _WR_SELF(uint32_t, seed, seed + 13);
              iter, seed;
            }
          }
        }
        _WR_SELF(unsigned int, func, func + 1);
      }
    }

    if (n_0 == 502 && n_1 == 502 && n_2 == 502 && n_3 == 502 && n_4 == 502 &&
        n_5 == 502 && n_6 == 502) {
      // ok
    } else {
      // wrong
    }
  }
  _end(bc_main);
}

_gdef static immortal_thread_t threads[1];

int main(int argc, char *argv[]) {
  _imt_thread_init_macro(&threads[0], bc_main, ((void *)0) /*NULL*/);
  imt_run(threads, 1, 0 /*false*/);
  return 0;
}

