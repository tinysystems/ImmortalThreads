#include "immortality.h"

#include <stdio.h>

_immortal_function(do_while_loop_test) {
  _begin(do_while_loop_test);
  _def size_t i;
  _WR(i, 100);
  {
    _def char condition_tmp;
    do {
      _WR_SELF(size_t, i, i - 1);
      _WR(condition_tmp, i != 0);
    } while (condition_tmp);
  }

  /**
   * No decomposition expected
   */
  do {
    _WR_SELF(size_t, i, i - 1);
  } while (i != 0);
  _end(do_while_loop_test);
}

