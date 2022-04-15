/**
 * \test A variable that is already put into a section should not be
 * instrumented
 */
#include "immortality.h"

__hifram int b = 1;

void test() {
}
