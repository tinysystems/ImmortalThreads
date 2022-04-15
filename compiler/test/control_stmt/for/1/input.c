#include "immortality.h"

void for_loop_test() {
  for (size_t foo = 0; ++foo == 100;) {
    --foo;
    ++foo;
    --foo;
  }
  for (size_t foo = 0;; ++foo) {
    --foo;
    ++foo;
    --foo;
  }
}
