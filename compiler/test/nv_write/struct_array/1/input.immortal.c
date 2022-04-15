#include "immortality.h"

_immortal_function(test_struct_with_array_members) {
  _begin(test_struct_with_array_members);
  _def struct { uint8_t arr[10]; } data;

  while (1) {
    // expected: `_WR(data.arr[0], 1);`
    _WR(data.arr[0], 1);
    // expected: `_WR_SELF(uint8_t, data.arr[0], data.arr[0] + 1);`
    _WR_SELF(uint8_t, data.arr[0], data.arr[0] + 1);
  }
  _end(test_struct_with_array_members);
}

