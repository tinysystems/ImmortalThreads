/**
 * Test case: shim API replacement of imt_thread_init and immortal_thread
 */
#include "immortality.h"

#include <stdio.h>

static int bar(int a) { return 42; }

static immortal_thread(foo, args) {
  while (1) {
    bar(1);
  }
}

immortal_thread_t threads[3];

int main(void) {
  imt_thread_init(&threads[0], foo, NULL);
  imt_thread_init(&threads[1], foo, NULL);
  imt_thread_init(&threads[2], foo, NULL);
  imt_run(threads, 2, true);
}
