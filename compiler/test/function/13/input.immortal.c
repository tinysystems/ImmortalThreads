/**
 * Test case: multi instance function, global variables and parameters must not
 * be transformed into arrays
 */
#include "immortality.h"

_gdef int g = 0;

// annotation on the definition
_immortal_function_metadata_def(, test, 3);
_immortal_function(test, int hello) {
  _begin_multi(test, _id);
  _def int a[3];
  if (hello) {
    _WR_SELF(int, (a[_id]), (a[_id]) + 1);
    _WR_SELF(int, g, g + 1);
  }
  _end_multi(test);
}

