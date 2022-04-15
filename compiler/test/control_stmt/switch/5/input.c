/**
 * \test Switch statements with some free usage of break statement
 */
#include "immortality.h"

#include <stdio.h>

void switch_transformation_test() {
  size_t i = 0;
  switch (i) {
  case 0:
    i = 0;
    break;
  default:
    break;
  }

  switch (i) {
  case 0:
    break;
  default:
    i = 0;
    break;
  }

  switch (i) {
  case 2:
    i = 2;
    break;
    i = 1;
  default:
    i = 0;
    break;
  }

  switch (i) {
  case 0: {
    i = 0;
    break;
    i = 1;
  }
  default:
    i = 0;
    break;
  }
}
