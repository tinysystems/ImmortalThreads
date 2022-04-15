/**
 * Test case: interrupt handler should not be instrumented
 */
#include "immortality.h"

void __attribute__((interrupt(TIMER1_A0_VECTOR))) _Timer_PF(void) {
  int a = 0;
  a++;
}
