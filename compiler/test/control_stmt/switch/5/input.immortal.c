/**
 * \test Switch statements with some free usage of break statement
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function(switch_transformation_test) {
  _begin(switch_transformation_test);
  _def size_t i;
  _WR(i, 0);
  do {
    if (i == 0) {
      _WR(i, 0);
    } else {
      break;
    }
  } while (0);

  do {
    if (i == 0) {
      break;
    } else {
      _WR(i, 0);
    }
  } while (0);

  if (i == 2) {
    _WR(i, 2);
  } else {
    _WR(i, 0);
  }

  do {
    if (i == 0) {
      {
        _WR(i, 0);
        break;
        _WR(i, 1);
      };
    } else {
      _WR(i, 0);
    }
  } while (0);
  _end(switch_transformation_test);
}

