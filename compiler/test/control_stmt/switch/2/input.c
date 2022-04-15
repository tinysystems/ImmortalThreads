/**
 * \test Switch statement with mixed break and empty fallthrough
 */
#include "immortality.h"

#include <stdio.h>

void switch_transformation_test() {
  size_t i = 0;
  switch (i) {
  case 123:
    i = 123;
    break;
  case 0:
  case 1:
    i = 1;
    break;
  case 2:
  case 3:
    i = 1;
    break;
  default:
    i = 3;
    break;
  }
}
