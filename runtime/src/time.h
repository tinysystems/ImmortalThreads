/**
 * \file time.h
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 *
 * This is the file that reads the time from a persistent timekeeper. We also
 * provided a random value generator to imitate tge timing behavior.
 */

#ifndef IMMORTALITY_TIME_H_
#define IMMORTALITY_TIME_H_

#define TIME_SIMULATE

#ifdef TIME_SIMULATE
#include <stdlib.h>
#define GETTIME() rand()
#else
/* provide a time support */
uint32_t GETTIME();
#endif

#endif /* IMMORTALITY_TIME_H_ */
