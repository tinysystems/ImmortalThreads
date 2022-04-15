/**
 * \test Simple switch statement where every case has a break
 */
#include "immortality.h"

#include <stdio.h>

void switch_transformation_test() {
  size_t i = 0;
  switch (i) {
  case 0:
    i = 0;
    break;
  case 1:
    i = 1;
    break;
  default:
    i = 2;
    break;
  }
}
