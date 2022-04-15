#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def int a;
  _def int *b;
  _WR(b, &a);
  _def int **c;
  _WR(c, &b);

  _WR_SELF(int, a, a + 1);
  _WR_SELF(int *, b, b + 1);
  _WR_SELF(int **, c, c + 1);
  _WR_SELF(int *, (*c), (*c) + 1);
  _end(test);
}

