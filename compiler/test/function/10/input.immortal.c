/**
 * Test case: multiple return values
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function_with_retval(test, int) {
  _begin(test);
  _def int a;
  _WR(a, 0);
  if (a) {
    _return(test, 1);
  }
  _return(test, 0);
  _end(test);
}

