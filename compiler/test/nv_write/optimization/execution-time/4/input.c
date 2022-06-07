/**
 * \test _WR_SELF coalescence
 */
#include "immortality.h"

void test() {
  int a = 0;
  for (int i = 0, j = 0, k = 0; i < 10;) {
    ++i;
    ++j;
    // interruption in between
    a = j;
    j = a + 1;
    ++k;

    if (0) {
      // more than maximum supported coalescence
      ++i;
      ++j;
      ++k;

      ++i;
      ++j;
      ++k;

      ++i;
    }
  }
}
