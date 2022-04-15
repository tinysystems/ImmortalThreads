/**
 * Test case: functions declared in instrumentable header file should be
 * brought in the main source file, wrapped with the appropriate macros.
 */
#include "immortality.h"

#include "input_0.h"
#include "input_1.h"

void test_main(void) {
  foo();
  bar(1, 2);
}
