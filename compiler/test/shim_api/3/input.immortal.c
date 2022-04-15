/**
 * Test case: shim API replacement of OS primitives
 */
#include "immortality.h"

#include <stdio.h>

_gdef event_t event;
_gdef mutex_t mutex;
_gdef sem_t semaphore;

_immortal_function(test) {
  _begin(test);
  _EVENT_INIT(event);
  _EVENT_SET_TIMESTAMP(event, 123);
  {
    _def uint32_t time;
    _WR(time, _EVENT_GET_TIMESTAMP(event));
  }
  {
    _def uint8_t *buf;
    _WR(buf, _EVENT_GET_BUFFER(event));
  }
  _EVENT_SIGNAL(event);
  {
    _def uint8_t buffer[2];
    _EVENT_WAIT_EXPIRES(event, buffer, 123);
  }

  {
    _LOCK_INIT(mutex);
    _ENTER(mutex);
    _LEAVE(mutex);
  }
  {
    _SEM_INIT(semaphore);
    _SEM_WAIT(semaphore);
    _SEM_POST_ISR(semaphore);
  }
  _end(test);
}

