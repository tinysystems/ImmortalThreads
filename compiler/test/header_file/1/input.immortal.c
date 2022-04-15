/**
 * Test case: variables declared in non-instrumentable header files should not be instrumented
 */
#include "immortality.h"

#include "input.h"

int a = 5;
double b = 10.0;

// This is only declared in this file, so can be instrumented
_gdef int c = 5;

