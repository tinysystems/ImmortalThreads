#include "immortality.h"

void test_struct_with_array_members() {
  struct {
    uint8_t arr[10];
  } data;

  while (1) {
    // expected: `_WR(data.arr[0], 1);`
    data.arr[0] = 1;
    // expected: `_WR_SELF(uint8_t, data.arr[0], data.arr[0] + 1);`
    data.arr[0] = data.arr[0] + 1;
  }
}
