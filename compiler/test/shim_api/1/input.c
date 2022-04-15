/**
 * Test case: shim API replacement of multi instance thread
 */
#include "immortality.h"

#include <stdio.h>

immortalc_fn_max_instances(3) static immortal_thread(foo, args) {
  while (1) {
  }
}

immortal_thread_t threads[3];

int main(void) {
  imt_thread_init(&threads[0], foo, NULL);
  imt_thread_init(&threads[1], foo, NULL);
  imt_thread_init(&threads[2], foo, NULL);
  imt_run(threads, 3, true);
}
