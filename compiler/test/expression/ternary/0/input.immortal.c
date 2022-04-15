/**
 * \test Test case: ternary expression decomposition
 */
#include "immortality.h"

_immortal_function(test_ternary) {
  _begin(test_ternary);
  _def uint32_t a;
  _WR(a, 0);
  _def uint32_t b;
  _WR(b, 1);
  _def uint32_t c;
  _WR(c, 1);

  _WR(a, b >= 1 ? b : c);

  if (b >= 1) {
    _WR_SELF(uint32_t, b, b + 1);
  } else {
    c;
  };

  {
    _def unsigned int _ternary_tmp_var;
    if (b >= 1) {
      {
        _def uint32_t tmp_var_0;
        _WR(tmp_var_0, b);
        _WR_SELF(uint32_t, b, b + 1);
        _WR(_ternary_tmp_var, tmp_var_0);
      }
    } else {
      _WR(_ternary_tmp_var, c);
    }
    _WR(a, _ternary_tmp_var);
  }

  {
    _def unsigned int _ternary_tmp_var;
    if (b >= 1) {
      _WR(_ternary_tmp_var, b);
    } else {
      {
        _def uint32_t tmp_var_0;
        _WR(tmp_var_0, c);
        _WR_SELF(uint32_t, c, c + 1);
        _WR(_ternary_tmp_var, tmp_var_0);
      }
    }
    _WR(a, _ternary_tmp_var);
  }

  {
    _def unsigned int _ternary_tmp_var;
    {
      _def char condition_tmp;
      {
        _def uint32_t tmp_var_0;
        _WR(tmp_var_0, b);
        _WR_SELF(uint32_t, b, b + 1);
        _WR(condition_tmp, (tmp_var_0 >= 1));
      }
      if (condition_tmp) {
        _WR_SELF(uint32_t, b, b + 1);
        _WR(_ternary_tmp_var, b);
      } else {
        _WR_SELF(uint32_t, c, c + 1);
        _WR(_ternary_tmp_var, c);
      }
    }
    _WR(a, _ternary_tmp_var);
  }

  if (1) {
    {
      _def unsigned int _ternary_tmp_var;
      {
        _def char condition_tmp;
        {
          _def uint32_t tmp_var_0;
          _WR(tmp_var_0, b);
          _WR_SELF(uint32_t, b, b + 1);
          _WR(condition_tmp, (tmp_var_0 >= 1));
        }
        if (condition_tmp) {
          {
            _def char condition_tmp;
            _WR_SELF(uint32_t, b, b + 1);
            _WR(condition_tmp, (b));
            if (condition_tmp) {
              _WR_SELF(uint32_t, b, b + 1);
              _WR(_ternary_tmp_var, b);
            } else {
              _WR_SELF(uint32_t, c, c + 1);
              _WR(_ternary_tmp_var, c);
            }
          }
        } else {
          {
            _def char condition_tmp;
            _WR_SELF(uint32_t, c, c + 1);
            _WR(condition_tmp, (c));
            if (condition_tmp) {
              _WR_SELF(uint32_t, c, c + 1);
              _WR(_ternary_tmp_var, c);
            } else {
              _WR_SELF(uint32_t, b, b + 1);
              _WR(_ternary_tmp_var, b);
            }
          }
        }
      }
      _WR(a, _ternary_tmp_var);
    }
  }

  while (1) {
    if (b >= 1) {
      (b >= 2 ? b : c);
    } else {
      if (c >= 1) {
        _WR_SELF(uint32_t, c, c + 1);
      } else {
        _WR_SELF(uint32_t, b, b + 1);
      }
    };
  }
  _end(test_ternary);
}

