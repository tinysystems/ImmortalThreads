/**
 * \test _WR_SELF coalescence. Not everything can be coalesced
 */
#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def int i, j;
  _WR(i, 0);
  j = 0;
  _def int *p;
  p = &i;

  if (0) {
    // cannot coalesce
    _WR_SELF(int, i, i + 1);
    _WR_SELF(int, i, i + 1);
    _WR_SELF(int, i, i + 1);
  }

  if (0) {
    // cannot coalesce
    _WR_SELF(int, j, j + 1);
    _WR_SELF(int, j, j + 1);
    _WR_SELF(int, j, j + 1);
  }
  if (0) {
    // can only pairwise coalesce
    _WR_SELF_CONTEMPORANEOUS_2(int, i, i + 1, int, j, j + 1);
    _WR_SELF_CONTEMPORANEOUS_2(int, i, i + 1, int, j, j + 1);
  }

  if (0) {
    // cannot coalesce
    _WR_SELF(int, i, i + 1);
    _WR_SELF(int, j, j + i);
  }

  if (0) {
    // cannot coalesce
    _WR_SELF(int, i, i + j);
    _WR_SELF(int, j, j + i);
  }

  // can coalesce
  if (0) {
    _WR_SELF_CONTEMPORANEOUS_2(int, i, i + 1, int *, p, p + 1);
  }

  if (0) {
    // can coalesce since i and *p alias
    _WR_SELF(int, i, i + 1);
    _WR_SELF(int, (*p), (*p) + 1);
  }

  if (0) {
    // can coalesce since j and *p don't alias
    _WR_SELF_CONTEMPORANEOUS_2(int, j, j + 1, int, (*p), (*p) + 1);
  }
  _end(test);
}

