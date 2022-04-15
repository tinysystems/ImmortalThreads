/**
 * Test case: functions declared in instrumentable header file which are
 * not directly included
 */
#include "immortality.h"

#include "main.h"
_immortal_function_with_retval(bar, int, int a, int b);
_immortal_function(foo);

_immortal_function(test_main) {
  _begin(test_main);
  _call(foo);
  _call(bar, 0, 1, 2);
  _end(test_main);
}

