/**
 * Test case: nested function calls
 */
#include "immortality.h"

#include <stdio.h>

uint32_t foo(uint32_t a, uint32_t b) { return a + b; }
uint32_t bar(uint32_t a, uint32_t b) immortalc_fn_idempotent { return a + b; }
uint32_t baz(uint32_t a, uint32_t b) { return a + b; }

void test_nested_function_calls(void) {
  uint32_t sum;
  uint32_t a = 0;
  uint32_t b = 1;
  uint32_t c = 1;

  while (1) {
    foo(bar(a, b), baz(a, c));
    foo(c = bar(a, b), baz(a, b));
    foo(bar(a, b), c = baz(a, b));
  }
}
