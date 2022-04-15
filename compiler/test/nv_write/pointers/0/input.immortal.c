#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t arr[100];
  _WR(arr[99], (4294967295U)) /*UINT32_MAX*/;

  while (1) {
    _def uint32_t *it;
    _WR(it, arr);
    while (*it != (4294967295U) /*UINT32_MAX*/) {
      if (it != arr) {
        // assign to the current iterator +1 from the previous value
        _WR_SELF(uint32_t, *it, *(it - 1) + 1);
      }
      _WR_SELF(uint32_t *, it, it + 1);
    }
  }
  _end(test);
}

