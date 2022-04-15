/**
 * Test case: shim API replacement of OS primitives
 */
#include "immortality.h"

#include <stdio.h>

event_t event;
mutex_t mutex;
sem_t semaphore;

void test() {
  EVENT_INIT(event);
  EVENT_SET_TIMESTAMP(event, 123);
  { uint32_t time = EVENT_GET_TIMESTAMP(event); }
  { uint8_t *buf = EVENT_GET_BUFFER(event); }
  EVENT_SIGNAL(event);
  {
    uint8_t buffer[2];
    EVENT_WAIT_EXPIRES(event, buffer, 123);
  }

  {
    LOCK_INIT(mutex);
    ENTER(mutex);
    LEAVE(mutex);
  }
  {
    SEM_INIT(semaphore);
    SEM_WAIT(semaphore);
    SEM_POST_ISR(semaphore);
  }
}
