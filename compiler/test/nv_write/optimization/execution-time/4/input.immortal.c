/**
 * \test _WR_SELF coalescence
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def int a;
  _WR(a, 0);
  {
    _def int i, j, k;
    i = 0;
    j = 0;
    k = 0;
    for (; i < 10;) {
      _WR_SELF_CONTEMPORANEOUS_2(int, i, i + 1, int, j, j + 1);
      // interruption in between
      a = j;
      _WR(j, a + 1);
      _WR_SELF(int, k, k + 1);

      if (0) {
        // more than maximum supported coalescence
        _WR_SELF_CONTEMPORANEOUS_3(int, i, i + 1, int, j, j + 1, int, k, k + 1);

        _WR_SELF_CONTEMPORANEOUS_3(int, i, i + 1, int, j, j + 1, int, k, k + 1);

        _WR_SELF(int, i, i + 1);
      }
    }
  }
  _end(test);
}

