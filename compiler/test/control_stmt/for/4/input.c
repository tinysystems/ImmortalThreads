/**
 * \test For statement with init statement that initializes a variable from
 * outer scope
 */
#include "immortality.h"

void for_loop_test() {
  size_t i;
  for (i = 0; i < 6; ++i) {
  }
}
