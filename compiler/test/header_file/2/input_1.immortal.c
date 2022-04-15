#include "immortality.h"

#include "input_1.h"
_immortal_function_with_retval(bar, int, int a, int b);

_immortal_function_with_retval(bar, int, int a, int b) {
  _begin(bar);
  _return(bar, 1);
  _end(bar);
}

