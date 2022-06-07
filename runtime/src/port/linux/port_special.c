/**
 * \file port_special.c
 * \brief Platform-specific special interface that is exposed to the
 * application - implementation
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */

#include "port_special.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/personality.h>
#include <sys/stat.h>
#include <unistd.h>

#if 1
#define debug(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
// Why this `if (0)`? We want to prevent compiler's unused warnings
#define debug(fmt, ...)                                                        \
  if (0)                                                                       \
  fprintf(stderr, fmt, ##__VA_ARGS__)
#endif

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

/**
 * See linker script and
 * https://linux.die.net/man/3/edata
 * https://stackoverflow.com/questions/16552710/how-do-you-get-the-start-and-end-addresses-of-a-custom-elf-section
 * https://stackoverflow.com/questions/7370407/get-the-start-and-end-address-of-text-section-in-an-executable
 */
extern uint8_t __data_start;
extern uint8_t edata;
extern uint8_t __bss_start;
extern uint8_t end;
extern uint8_t __stack_start;
extern uint8_t __start_persistent;
extern uint8_t __stop_persistent;

static void checked_write(int fd, const void *buf, size_t n) {
  debug("Writing %p, length: %td\n", buf, n);
  for (size_t i = 0; i < n; ++i) {
    debug("0x%02x ", (unsigned)((uint8_t *)buf)[i]);
  }
  debug("\n");

  ssize_t res = write(fd, buf, n);

  if (res < 0 || (size_t)res != n) {
    if (res == -1) {
      handle_error("write");
    } else {
      handle_error("partial write");
    }
  }
}

static void checked_read(int fd, void *buf, size_t n) {
  debug("Reading %p, length: %td\n", buf, n);
  ssize_t res = read(fd, buf, n);

  for (size_t i = 0; i < n; ++i) {
    debug("0x%02x ", (unsigned)((uint8_t *)buf)[i]);
  }
  debug("\n");

  if (res < 0 || (size_t)res != n) {
    if (res == -1) {
      handle_error("read");
    } else {
      handle_error("partial read");
    }
  }
}

void im_port_linux_init(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /**
   * Disable address space randomization
   * https://askubuntu.com/a/1355819
   */
  const int old_personality = personality(ADDR_NO_RANDOMIZE);
  if (!(old_personality & ADDR_NO_RANDOMIZE)) {
    const int new_personality = personality(ADDR_NO_RANDOMIZE);
    if (new_personality & ADDR_NO_RANDOMIZE) {
      execv(argv[0], argv);
    }
  }

  int fd = open(argv[1], O_RDWR | O_CREAT, 0666);
  if (fd == -1) {
    handle_error("open");
  }

  debug(".data start: %p\n"
        ".data end: %p\n"
        ".data size: %td\n",
        &__data_start, &edata, &edata - &__data_start);

  debug(".bss start: %p\n"
        ".bss end: %p\n"
        ".bss size: %td\n",
        &__bss_start, &end, &end - &__bss_start);

  debug(".persistent start: %p\n"
        ".persistent end: %p\n"
        ".persistent size: %td\n",
        &__start_persistent, &__stop_persistent,
        &__stop_persistent - &__start_persistent);

  void *static_memory_start = &__data_start;
  ssize_t static_memory_len = &end - &__data_start;
  void *persistent_memory_start = &__start_persistent;
  ssize_t persistent_memory_len = &__stop_persistent - &__start_persistent;

  {
    struct stat sb;

    if (fstat(fd, &sb) == -1) {
      handle_error("fstat");
    }
    if (sb.st_size == 0) {
      debug("Empty file. Writing...\n");
      checked_write(fd, static_memory_start, static_memory_len);
    } else if (sb.st_size == static_memory_len) {
      debug("Filled file. Restoring non volatile memory\n");
      // first read non volatile section from file
      if (lseek(fd, persistent_memory_start - static_memory_start, SEEK_SET) ==
          -1) {
        handle_error("lseek");
      }
      checked_read(fd, persistent_memory_start, persistent_memory_len);

      /*
       * then write all sections to file, so that volatile sections are set
       * to their initial values, simulating volatility
       */
      if (lseek(fd, 0, SEEK_SET) == -1) {
        handle_error("lseek");
      }
      checked_write(fd, static_memory_start, static_memory_len);
    } else {
      fprintf(stderr,
              "Filled file, but with unexpected dimension %lu instead of %lu. "
              "Abort.\n",
              sb.st_size, static_memory_len);
      exit(EXIT_FAILURE);
    }
  }

  /**
   * Inspiration
   * https://stackoverflow.com/questions/64308319/mapping-existing-memory-data-segment-to-another-memory-segment
   */
  void *addr = mmap(static_memory_start, static_memory_len,
                    PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    handle_error("mmap");
  }
  debug("Mapped address %p\n", addr);

  /*
   * NEVER munmap, since it would clobber our file mapped sections,
   * which woud lead the program to crash.
   */
}
