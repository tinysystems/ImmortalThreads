#include "immortality.h"

#include <stdio.h>

#define INIT()                                                                 \
  do {                                                                         \
    int a = 0;                                                                 \
  } while (0)
#define INCREMENT()                                                            \
  do {                                                                         \
    int a = 0;                                                                 \
    a++;                                                                       \
  } while (0)

void macro_test() {
  INIT();
  INCREMENT();
}
