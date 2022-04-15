/**
 * \test A variable that is already put into a section should not be
 * instrumented
 */
#include "immortality.h"

__attribute__((section(".persistent_hifram "))) /*__hifram*/ int b = 1;

_immortal_function(test) {
  _begin(test);
  _end(test);
}

