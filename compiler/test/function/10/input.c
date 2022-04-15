/**
 * Test case: multiple return values
 */
#include "immortality.h"

#include <stdio.h>

int test(void) {
  int a = 0;
  if (a) {
    return 1;
  }
  return 0;
}
