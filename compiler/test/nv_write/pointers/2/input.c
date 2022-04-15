#include "immortality.h"

void test(void) {
  int a;
  int *b = &a;
  int **c = &b;

  ++a;
  ++b;
  ++c;
  ++(*c);
}
