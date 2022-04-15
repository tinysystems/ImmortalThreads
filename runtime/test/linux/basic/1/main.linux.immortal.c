#include "immortality.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define NUMBER_OF_INSTANCES 2

_immortal_function_metadata_def(static, increment_0, 2);
_immortal_function(increment_0, int *num) {
  _begin_multi(increment_0, _id);
  _WR_SELF(int, *num, *num + 2);
  _def uint32_t dumb[2];
  _WR((dumb[_id]), 1);
  for (; (dumb[_id]) != (4294967295U) /*UINT32_MAX*/ / 1000;) {
    // loop until reached number
    _WR_SELF(uint32_t, (dumb[_id]), (dumb[_id]) + 1);
  }
  _end_multi(increment_0);
}

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

_immortal_function_metadata_def(static, test_thread_0, 2);
_immortal_function(test_thread_0, void *args) {
  _begin_multi(test_thread_0, _id);
  _def int persistent_cnt[2];
  _WR((persistent_cnt[_id]), 0);
  _def const char *color[2];
  if (strcmp(args, "hello")) {
    _WR((color[_id]), "\x1b[32m") /*ANSI_COLOR_GREEN*/;
  } else {
    _WR((color[_id]), "\x1b[33m") /*ANSI_COLOR_YELLOW*/;
  }

  while (1) {
    _call(increment_0, &(persistent_cnt[_id]));
    printf("%s[thread: %s] persistent cnt %d"
           "\x1b[0m" /*ANSI_COLOR_RESET*/ "\n",
           (color[_id]), (const char *)args, (persistent_cnt[_id]));
    _CHECKPOINT();
    usleep(50000);
    _CHECKPOINT();
  }
  _end_multi(test_thread_0);
}

_immortal_function_metadata_def(static, increment_1, 2);
_immortal_function_with_retval(increment_1, int, int num) {
  _begin_multi(increment_1, _id);
  _def uint32_t dumb[2];
  _WR((dumb[_id]), 1);
  for (; (dumb[_id]) != (4294967295U) /*UINT32_MAX*/ / 1000;) {
    // loop until reached number
    _WR_SELF(uint32_t, (dumb[_id]), (dumb[_id]) + 1);
  }
  _return(increment_1, num + 2);
  _end_multi(increment_1);
}

_immortal_function_metadata_def(static, test_thread_1, 2);
_immortal_function(test_thread_1, void *args) {
  _begin_multi(test_thread_1, _id);
  _def int persistent_cnt[2];
  _WR((persistent_cnt[_id]), 0);
  while (1) {
    _call(increment_1, &(persistent_cnt[_id]), (persistent_cnt[_id]));
    printf("\x1b[34m" /*ANSI_COLOR_BLUE*/
           "[thread: singleton] persistent cnt %d"
           "\x1b[0m" /*ANSI_COLOR_RESET*/ "\n",
           (persistent_cnt[_id]));
    _CHECKPOINT();
    usleep(50000);
  }
  _end_multi(test_thread_1);
}

_gdef static immortal_thread_t threads[3];

int main(int argc, char *argv[]) {
  im_port_linux_init(argc, argv);
  char *thread_1 = "hello";
  char *thread_2 = "world";

  _imt_thread_init_multi_macro(&threads[0], test_thread_0, thread_1);
  _imt_thread_init_multi_macro(&threads[1], test_thread_0, thread_2);
  _imt_thread_init_multi_macro(&threads[2], test_thread_1,
                               ((void *)0) /*NULL*/);
  imt_run(threads, 3, 1 /*true*/);
  return 0;
}
