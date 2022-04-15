/**
 * Test case: _begin and _end instrumentation
 */
#include "immortality.h"

_immortal_function(sense_temp) {
  _begin(sense_temp);
  // expected _begin(SENSE_TEMP);
  // expected _end(SENSE_TEMP);
  _end(sense_temp);
}

