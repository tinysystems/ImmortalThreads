/**
 * \test The switch statement's control expression contains a side effect.
 * The transformation should ensure that the side effect happens only once.
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function(switch_transformation_test) {
  _begin(switch_transformation_test);
  _def size_t i;
  _WR(i, 0);
  {
    _def size_t condition_tmp;
    {
      _def size_t tmp_var_0;
      _WR(tmp_var_0, i);
      _WR_SELF(size_t, i, i + 1);
      _WR(condition_tmp, (tmp_var_0));
    }
    if (condition_tmp == 0) {
      _WR(i, 0);
    } else if (condition_tmp == 1) {
      _WR(i, 1);
    } else {
      _WR(i, 2);
    }
  }
  _end(switch_transformation_test);
}

