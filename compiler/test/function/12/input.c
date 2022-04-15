/**
 * Test case: multi instance function
 */
#include "immortality.h"

// annotation on the declaration
void foo(void) immortalc_fn_max_instances(3);
// the definition should be treated as multi-instance function
void foo(void) {

}

// annotation on the definition
void test(void) immortalc_fn_max_instances(3) {
  int a = 0;
  a++;

  int aa = 0;
  aa++;

  int aaaaa = 0;
  aaaaa++;
}
