/**
 * \file main.c
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#include "immortality.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define NUMBER_OF_INSTANCES 2

static void increment_0(int *num) {
  *num += 2;
  uint32_t dumb;
  dumb = 1;
  for (; dumb != UINT32_MAX / 1000;) {
    // loop until reached number
    ++dumb;
  }
}

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

static immortal_thread(test_thread_0, args) {
  int persistent_cnt = 0;
  const char *color;
  if (strcmp(args, "hello")) {
    color = ANSI_COLOR_GREEN;
  } else {
    color = ANSI_COLOR_YELLOW;
  }

  while (1) {
    increment_0(&persistent_cnt);
    printf("%s[thread: %s] persistent cnt %d" ANSI_COLOR_RESET "\n", color,
           (const char *)args, persistent_cnt);
    CHECKPOINT();
    usleep(50000);
    CHECKPOINT();
  }
}

static int increment_1(int num) {
  uint32_t dumb;
  dumb = 1;
  for (; dumb != UINT32_MAX / 1000; ++dumb) {
    // loop until reached number
  }
  return num + 2;
}

static immortal_thread(test_thread_1, args) {
  int persistent_cnt = 0;
  while (1) {
    persistent_cnt = increment_1(persistent_cnt);
    printf(ANSI_COLOR_BLUE
           "[thread: singleton] persistent cnt %d" ANSI_COLOR_RESET "\n",
           persistent_cnt);
    CHECKPOINT();
    usleep(50000);
  }
}

static immortal_thread_t threads[3];

int main(int argc, char *argv[]) {
  im_port_linux_init(argc, argv);
  char *thread_1 = "hello";
  char *thread_2 = "world";

  imt_thread_init(&threads[0], test_thread_0, thread_1);
  imt_thread_init(&threads[1], test_thread_0, thread_2);
  imt_thread_init(&threads[2], test_thread_1, NULL);
  imt_run(threads, 3, true);
  return 0;
}
