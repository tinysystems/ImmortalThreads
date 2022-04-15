/**
 * Test case: functions declared in instrumentable header file which are
 * not directly included
 */
#include "immortality.h"

#include "main.h"

void test_main(void) {
  foo();
  bar(1, 2);
}
