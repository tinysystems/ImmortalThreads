/**
 * \test _WR_SELF coalescence. Not everything can be coalesced
 */
#include "immortality.h"

void test() {
  int i = 0, j = 0;
  int *p = &i;

  if (0) {
    // cannot coalesce
    ++i;
    ++i;
    ++i;
  }

  if (0) {
    // cannot coalesce
    ++j;
    ++j;
    ++j;
  }
  if (0) {
    // can only pairwise coalesce
    ++i;
    ++j;
    ++i;
    ++j;
  }

  if (0) {
    // cannot coalesce
    ++i;
    j += i;
  }

  if (0) {
    // cannot coalesce
    i += j;
    j += i;
  }

  // can coalesce
  if (0) {
    ++i;
    ++p;
  }

  if (0) {
    // can coalesce since i and *p alias
    ++i;
    ++(*p);
  }

  if (0) {
    // can coalesce since j and *p don't alias
    ++j;
    ++(*p);
  }
}
