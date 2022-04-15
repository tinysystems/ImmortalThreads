/**
 * \test Writes to immortal variables manually specified by the programmer
 * should also be instrumented.
 */
#include "immortality.h"

#define decl_a_b(_prefix_)                                                     \
  __gdef int _prefix_##_a = 1;                                                 \
  __gdef int _prefix_##_b = 1;

__attribute__((section(".persistent_hifram "))) int hello_a = 1;
__attribute__((section(".persistent_hifram "))) int hello_b = 1;
    /*decl_a_b*/ /*(hello)*/
;

__attribute__((section(".persistent_hifram "))) /*__gdef*/ int b = 1;
__attribute__((section(".upper.rodata"))) /*__ro_hifram*/ const int c = 1;

_immortal_function(test) {
  _begin(test);
  _WR(hello_a, 1);
  _WR(hello_b, 1);
  _WR(b, 2);
  _end(test);
}

