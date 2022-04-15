#include "immortality.h"

void test(void) {
  uint32_t arr[100];
  arr[99] = UINT32_MAX;

  while (1) {
    uint32_t *it = arr;
    while (*it != UINT32_MAX) {
      if (it != arr) {
        // assign to the current iterator +1 from the previous value
        *it = *(it - 1) + 1;
      }
      it++;
    }
  }
}
