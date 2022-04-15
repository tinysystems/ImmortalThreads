#include "immortality.h"

#include <stdio.h>

void if_decomposition_test() {
  size_t i = 0;
  if (++i == 1) {
    i = 0;
  }

  if (i++ == 1) {
    i = 0;
  }

  // should not be instrumented
  if (i == 1) {
    i = 0;
  }
}
