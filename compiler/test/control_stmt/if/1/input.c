#include "immortality.h"

#include <stdio.h>

void if_wihout_brace_test() {
  size_t i = 100;
  // Intentional misleading indentation
  // clang-format off
  if (true)
    i = 0;
    i = 0;
  // clang-format on
}
