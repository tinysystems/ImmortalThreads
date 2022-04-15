#include "immortality.h"

#include <stdio.h>

_immortal_function(while_loop_test) {
  _begin(while_loop_test);
  _def size_t i;
  _WR(i, 100);
  {
    _def char condition_tmp;
    _WR_SELF(size_t, i, i - 1);
    _WR(condition_tmp, (i != 0));
    while (condition_tmp) {
      // do stuff
      _WR_SELF(size_t, i, i - 1);
      _WR(condition_tmp, i != 0);
    }
  }
  _end(while_loop_test);
}

