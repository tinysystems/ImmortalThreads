/**
 * Test case: functions declared in instrumentable header file should be
 * brought in the main source file, wrapped with the appropriate macros.
 */
#include "immortality.h"

#include "input_0.h"
_immortal_function(foo);
#include "input_1.h"
_immortal_function_with_retval(bar, int, int a, int b);

_immortal_function(test_main) {
  _begin(test_main);
  _call(foo);
  _call(bar, 0, 1, 2);
  _end(test_main);
}

