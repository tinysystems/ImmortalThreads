/**
 * Test case: simple function calls with
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function(bar) {
  _begin(bar);
  _end(bar);
}

_immortal_function_with_retval(baz, int, int a) {
  _begin(baz);
  _return(baz, a);
  _end(baz);
}

_immortal_function(TEST) {
  _begin(TEST);
  _call(bar);

  _def int a;
  _def int c;
  _call(baz, &c, a);
  _call(baz, 0, a);
  _end(TEST);
}

