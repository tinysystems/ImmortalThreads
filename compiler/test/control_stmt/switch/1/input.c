/**
 * \test Simple switch statement made entirely of empty fallthrough
 */
#include "immortality.h"

#include <stdio.h>

void switch_transformation_test() {
  size_t i = 0;
  switch (i) {
  case 0:
  case 1:
  case 2:
  default:
    i = 3;
    break;
  }
}
