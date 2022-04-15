/**
 * Test case: shim API replacement of CHECKPOINT()
 */
#include "immortality.h"

#include <stdio.h>

void test() {
  int a;

  a += 0;
  CHECKPOINT();
  a += 0;
}
