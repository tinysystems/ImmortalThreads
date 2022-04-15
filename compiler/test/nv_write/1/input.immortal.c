#include "immortality.h"

_immortal_function(test) {
  _begin(test);
  _def uint32_t sum;
  // expected: _WR_SELF(uint32_t, sum, sum + 1);
  _WR_SELF(uint32_t, sum, sum + 1);
  // expected: _WR_SELF(uint32_t, sum, sum + 1);
  _WR_SELF(uint32_t, sum, sum + 1);
  // expected: _WR_SELF(uint32_t, sum, sum - 1):
  _WR_SELF(uint32_t, sum, sum - 1);
  // expected: _WR_SELF(uint32_t, sum, sum - 1);
  _WR_SELF(uint32_t, sum, sum - 1);
  // expected: _WR_SELF(uint32_t, sum, sum + 2);
  _WR_SELF(uint32_t, sum, sum + 2);
  // expected: _WR_SELF(uint32_t, sum, sum - 2):
  _WR_SELF(uint32_t, sum, sum - 2);
  // expected: _WR_SELF(uint32_t, sum, sum * 2);
  _WR_SELF(uint32_t, sum, sum * 2);
  // expected: _WR_SELF(uint32_t, sum, sum / 2):
  _WR_SELF(uint32_t, sum, sum / 2);
  // expected: _WR_SELF(uint32_t, sum, sum % 2);
  _WR_SELF(uint32_t, sum, sum % 2);
  // expected: _WR_SELF(uint32_t, sum, sum ^ 2);
  _WR_SELF(uint32_t, sum, sum ^ 2);
  // expected: _WR_SELF(uint32_t, sum, sum & 2);
  _WR_SELF(uint32_t, sum, sum & 2);
  // expected: _WR_SELF(uint32_t, sum, sum | 2);
  _WR_SELF(uint32_t, sum, sum | 2);
  // expected: _WR_SELF(uint32_t, sum, sum >> 2);
  _WR_SELF(uint32_t, sum, sum >> 2);
  // expected: _WR_SELF(uint32_t, sum, sum << 2);
  _WR_SELF(uint32_t, sum, sum << 2);
  _end(test);
}

