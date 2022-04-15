#include "immortality.h"

void for_loop_test() {
  size_t i = 0;
  /*
   * shouldn't bring `i` in init stmt position out, since it is not a
   * declaration
   */
  for (i; i < 6; ++i) {
  }
}
