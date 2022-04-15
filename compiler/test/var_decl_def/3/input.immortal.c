/**
 * \test Variabless with different scopes, visibility, lifetime and volatility
 */
#include "immortality.h"

/// Non volatile variable with external linkage
/// Expected _gdef int a = 0;
_gdef int a = 0;
/// Non volatile variable with internal linkage
/// Expected _gdef static int b = 0;
_gdef static int b = 1;

_immortal_function(test) {
  _begin(test);
  /// Non volatile local variable
  /// Expected _def int b = 0;
  _def int c;
  _WR(c, 0);
  _end(test);
}

