/**
 * \test Test case: same parameter and return value
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function_with_retval(foo, int, int a) {
  _begin(foo);
  _return(foo, a + 1);
  _end(foo);
}

_immortal_function(TEST) {
  _begin(TEST);
  _def int a;
  _WR(a, 0);
  _call(foo, &a, a);
  if (1) {
    // inside nested block
    _call(foo, &a, a);
  }
  _end(TEST);
}

