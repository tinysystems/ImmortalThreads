/**
 * Test case: shim API replacement of imt_thread_init and immortal_thread
 */
#include "immortality.h"

#include <stdio.h>

_immortal_function_with_retval(bar, int, int a) {
  _begin(bar);
  _return(bar, 42);
  _end(bar);
}

_immortal_function(foo, void *args) {
  _begin(foo);
  while (1) {
    _call(bar, 0, 1);
  }
  _end(foo);
}

_gdef immortal_thread_t threads[3];

int main(void) {
  _imt_thread_init_macro(&threads[0], foo, ((void *)0) /*NULL*/);
  _imt_thread_init_macro(&threads[1], foo, ((void *)0) /*NULL*/);
  _imt_thread_init_macro(&threads[2], foo, ((void *)0) /*NULL*/);
  imt_run(threads, 2, 1 /*true*/);
}

