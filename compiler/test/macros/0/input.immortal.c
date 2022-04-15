#include "immortality.h"

#include <stdio.h>

#define INIT()                                                                 \
  do {                                                                         \
    int a = 0;                                                                 \
  } while (0)
#define INCREMENT()                                                            \
  do {                                                                         \
    int a = 0;                                                                 \
    a++;                                                                       \
  } while (0)

_immortal_function(macro_test) {
  _begin(macro_test);
  do {
    _def int a;
    _WR(a, 0);
  } while (0) /*INIT*/ /*()*/;
  do {
    _def int a;
    _WR(a, 0);
    _WR_SELF(int, a, a + 1);
  } while (0) /*INCREMENT*/ /*()*/;
  _end(macro_test);
}

