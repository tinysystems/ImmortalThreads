/**
 * \file port.c
 * \brief Porting layer for Linux - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "../../sched.h"
#include "port/portable.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>

__fram static jmp_buf g_jmp_buf;

/* holds the address of the current thread index */
__fram static volatile uint8_t g_current_thread = 0;

static sigset_t g_all_signals;

static void fatal_error(const char *pcCall, int iErrno) {
  fprintf(stderr, "%s: %s\n", pcCall, strerror(iErrno));
  abort();
}

static void timeout_handler(int sig) { longjmp(g_jmp_buf, 1); }

void im_port_init(bool use_timer) {
  sigfillset(&g_all_signals);

  // interrupt will be enabled in im_port_run
  _di();

  if (use_timer) {
    struct itimerval itimer;
    int ret_val;

    /* Initialise the structure with the current timer information. */
    ret_val = getitimer(ITIMER_REAL, &itimer);
    if (ret_val) {
      fatal_error("getitimer", errno);
    }

    /* Set the interval between timer events. */
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = IMMORTALITY_PREEMPTION_MS * 1000;

    /* Set the current count-down. */
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = IMMORTALITY_PREEMPTION_MS * 1000;

    /* Set-up the timer interrupt. */
    ret_val = setitimer(ITIMER_REAL, &itimer, NULL);
    if (ret_val) {
      fatal_error("setitimer", errno);
    }

    struct sigaction sig_timeout;
    sig_timeout.sa_flags = 0;
    sig_timeout.sa_handler = timeout_handler;
    sigfillset(&sig_timeout.sa_mask);

    ret_val = sigaction(SIGALRM, &sig_timeout, NULL);
    if (ret_val) {
      fatal_error("sigaction", errno);
    }
  }
}

void im_port_run(immortal_thread_t *volatile threads, uint8_t size) {
  assert(size > 0 && "At least one thread should be passed");

  /* always update __th since it might be partially updated due to a power
   * failure */
  if (g_current_thread == size) {
    g_current_thread = 0;
  }

  int ret = setjmp(g_jmp_buf);

  _di();
  if ((g_current_thread += ret) == size) {
    g_current_thread = 0;
  }
  _ei();
  while (1) {
    imt_priv_run_thread(&threads[g_current_thread]);
    if ((++g_current_thread) == size) {
      g_current_thread = 0;
    }
  }
}

void im_enable_interrupt(void) {
  if (pthread_sigmask(SIG_UNBLOCK, &g_all_signals, NULL) != 0) {
    fatal_error("sigaction", errno);
  }
}
void im_disable_interrupt(void) {
  if (pthread_sigmask(SIG_BLOCK, &g_all_signals, NULL) != 0) {
    fatal_error("sigaction", errno);
  }
}
