/**
 * Test case: multi instance function, global variables and parameters must not
 * be transformed into arrays
 */
#include "immortality.h"

int g = 0;

// annotation on the definition
void test(int hello) immortalc_fn_max_instances(3) {
  int a;
  if (hello) {
    ++a;
    ++g;
  }
}
