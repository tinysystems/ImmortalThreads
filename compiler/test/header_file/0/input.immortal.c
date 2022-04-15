/**
 * Test case: functions declared in non-instrumentable header file should not
 * be instrumented.
 * We expect that immortalc outputs this file as is.
 */
#include "input.h"

void test(void) {
  uint32_t f = 0;
  f = 5;
}

