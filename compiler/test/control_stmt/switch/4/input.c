/**
 * \test The case statements contain compound statements
 */
#include "immortality.h"

#include <stdio.h>

void switch_transformation_test() {
  size_t i = 0;
  switch (i) {
  case 0: {
    int a = 0;
    i = 0;
  } break;
  case 1:
    i = 1;
    break;
  case 2: {
    i = 2;
  } break;
  }
}
