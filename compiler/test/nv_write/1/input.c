#include "immortality.h"

void test() {
  uint32_t sum;
  // expected: _WR_SELF(uint32_t, sum, sum + 1);
  sum++;
  // expected: _WR_SELF(uint32_t, sum, sum + 1);
  ++sum;
  // expected: _WR_SELF(uint32_t, sum, sum - 1):
  sum--;
  // expected: _WR_SELF(uint32_t, sum, sum - 1);
  --sum;
  // expected: _WR_SELF(uint32_t, sum, sum + 2);
  sum += 2;
  // expected: _WR_SELF(uint32_t, sum, sum - 2):
  sum -= 2;
  // expected: _WR_SELF(uint32_t, sum, sum * 2);
  sum *= 2;
  // expected: _WR_SELF(uint32_t, sum, sum / 2):
  sum /= 2;
  // expected: _WR_SELF(uint32_t, sum, sum % 2);
  sum %= 2;
  // expected: _WR_SELF(uint32_t, sum, sum ^ 2);
  sum ^= 2;
  // expected: _WR_SELF(uint32_t, sum, sum & 2);
  sum &= 2;
  // expected: _WR_SELF(uint32_t, sum, sum | 2);
  sum |= 2;
  // expected: _WR_SELF(uint32_t, sum, sum >> 2);
  sum >>= 2;
  // expected: _WR_SELF(uint32_t, sum, sum << 2);
  sum <<= 2;
}
