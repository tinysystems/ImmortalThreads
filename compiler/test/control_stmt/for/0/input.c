#include "immortality.h"

void for_loop_test() {
  for (size_t i = 0; i < 6; ++i) {
  }

  for (size_t i = 0, j = 0; i < 6; ++i, ++j) {
    for (size_t k = 0, l = 0; k < 6; ++k, ++l) {
    }
  }

  // typical infinite for loop that doesn't need to be decomposed
  size_t foo = 0;
  for (;;) {
    foo++;
  }
}
