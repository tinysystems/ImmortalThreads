/**
 * Test case: instrument function declarations
 */
#include "immortality.h"

#include <stdio.h>

void bar();
void bar();
int baz(int a);
int baz(int a);

void bar() {}

int baz(int a) { return a; }

void TEST(void) {
  bar();

  int a;
  int c = baz(a);
  baz(a);
}
