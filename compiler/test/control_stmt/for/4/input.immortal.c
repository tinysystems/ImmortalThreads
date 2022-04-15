/**
 * \test For statement with init statement that initializes a variable from
 * outer scope
 */
#include "immortality.h"

_immortal_function(for_loop_test) {
  _begin(for_loop_test);
  _def size_t i;
  {
    _WR(i, 0);
    for (; i < 6;) {
      _WR_SELF(size_t, i, i + 1);
    }
  }
  _end(for_loop_test);
}

