#include "immortality.h"

#include <stdio.h>

void do_while_loop_test() {
  size_t i = 100;
  do {
  } while (--i != 0);

  /**
   * No decomposition expected
   */
  do {
    --i;
  } while (i != 0);
}
