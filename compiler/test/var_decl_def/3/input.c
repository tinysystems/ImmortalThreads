/**
 * \test Variabless with different scopes, visibility, lifetime and volatility
 */
#include "immortality.h"

/// Non volatile variable with external linkage
/// Expected _gdef int a = 0;
int a = 0;
/// Non volatile variable with internal linkage
/// Expected _gdef static int b = 0;
static int b = 1;

void test() {
  /// Non volatile local variable
  /// Expected _def int b = 0;
  int c = 0;
}
