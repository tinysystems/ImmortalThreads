/**
 * Test case: shim API replacement of multi instance thread
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function_metadata_def(static, foo, 3);
_immortal_function(foo, void *args) {
  _begin_multi(foo, _id);
  while (1) {
  }
  _end_multi(foo);
}

_gdef immortal_thread_t threads[3];

int main(void) {
  _imt_thread_init_multi_macro(&threads[0], foo, ((void *)0) /*NULL*/);
  _imt_thread_init_multi_macro(&threads[1], foo, ((void *)0) /*NULL*/);
  _imt_thread_init_multi_macro(&threads[2], foo, ((void *)0) /*NULL*/);
  imt_run(threads, 3, 1 /*true*/);
}

