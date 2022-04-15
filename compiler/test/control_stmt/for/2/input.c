#include "immortality.h"

#include <stdio.h>

void for_loop_without_braces_test() {
  for (size_t i = 0; i < 6; ++i)
    i--;

  for (size_t i = 0; ++i < 6;)
    i--;
}
