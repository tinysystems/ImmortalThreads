/**
 * Test case: struct with array members
 */
#include "immortality.h"

void test_array_of_structs_with_array_members() {
  struct {
    uint8_t arr[10];
  } data[10];

  while (1) {
    // expected: `_WR(data[0].arr[0], 1);`
    data[0].arr[0] = 1;
    // expected: `_WR_SELF(uint8_t, data[0].arr[0], data.arr[0] + 1);`
    data[0].arr[0] = data[0].arr[0] + 1;
  }
}
