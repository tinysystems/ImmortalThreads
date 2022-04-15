#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t a;
  _WR(a, 0);
  _def uint32_t b;
  _WR(b, 1);
  _def uint32_t c;
  _WR(c, 1);
  _def uint32_t *p;
  _WR(p, &a);

  while (1) {
    _WR_SELF(uint32_t *, (p), (p) + 1);
    _WR_SELF(uint32_t *, (p), (p) + 1);
    _WR_SELF(uint32_t, (*(p)), (*(p)) + 1);
    _WR(a, (*(p)));
    _WR_SELF(uint32_t, (*(p)), (*(p)) + 1);
    a, (*(p));
    {
      _def uint32_t tmp_var_0;
      _WR(tmp_var_0, c);
      _WR_SELF(uint32_t, c, c + 1);
      _def uint32_t tmp_var_1;
      _WR(tmp_var_1, c);
      _WR_SELF(uint32_t, c, c + 1);
      _WR(a, tmp_var_0);
      _WR(a, tmp_var_1);
      _WR_SELF(uint32_t, b, b + 1);
      _WR_SELF(uint32_t, c, c + 1);
      a, a, b, c;
    }
    {
      _def uint32_t tmp_var_0;
      _WR(tmp_var_0, b);
      _WR_SELF(uint32_t, b, b + 1);
      _WR_SELF(uint32_t, c, c + 1);
      _WR(a, tmp_var_0 + c);
    }
  }
  _end(test);
}

