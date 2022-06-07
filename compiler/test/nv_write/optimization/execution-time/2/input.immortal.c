/**
 * \test _WR coalescence in array access and struct access
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def struct str {
    uint8_t a;
    uint32_t b;
    struct {
      uint8_t a;
      uint32_t b;
      uint32_t c;
    } c[10];
  } data[10];

  while (1) {
    if (1) {
      // different members
      _WR(data[0].a, data[1].b + 1);
      _WR(data[1].b, data[2].c[0].a + 1);
      data[2].c[0].b = data[3].c[0].c + 1;
    }

    if (1) {
      // same member, different indexes
      _WR(data[1].c[0].b, data[0].c[0].b + data[0].c[0].b);
      _WR(data[0].c[0].b, data[1].c[0].b + 1);

      data[0].c[1].b = data[1].c[0].b + data[0].c[0].b;
      _WR(data[0].c[0].b, data[0].c[1].b + 1);

      data[0].c[0].b = data[1].c[1].b + 1;
    }

    if (1) {
      // same member, non-constant indexes
      _def int i;
      _WR(i, 0);
      _WR_SELF_CONTEMPORANEOUS_2(uint32_t, data[i].c[i].b,
                                 data[i + 1 - 1].c[i + 1 - 1].b + 1, int, i,
                                 i - 1);
      data[i + 2].c[i + 2].b = data[i + 1].c[i + 1].b;
    }

    if (1) {
      _def struct str *ptr;
      _WR(ptr, &data[0]);
      // pointer "base", different member
      data[0].c[0].b = ptr->c[1].b + 1;
      data[0].c[1].c = ptr->c[0].c + 1;

      // pointer "base", same member
      _WR_SELF_CONTEMPORANEOUS_2(uint32_t, data[9].c[0].b, ptr->c[0].b + 1,
                                 uint32_t, data[9].c[0].c, ptr->c[0].c + 1);
    }
  }
  _end(test);
}

