/**
 * Test case: variables declared in instrumentable header files should not
 * be instrumented.
 * Only the definition shuold be instrumented.
 */
#include "immortality.h"

#include "input.h"

int a;
