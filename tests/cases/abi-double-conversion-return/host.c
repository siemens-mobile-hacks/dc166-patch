#include "test.h"

extern double signed_to_double(test_i32 value);
extern double unsigned_to_double(test_u32 value);

static test_i32 signed_round_trip(test_i32 value)
{
  return (test_i32)signed_to_double(value);
}

static test_u32 unsigned_round_trip(test_u32 value)
{
  return (test_u32)unsigned_to_double(value);
}

void main(void)
{
  test_check(1U, signed_round_trip(-123456789L) == -123456789L);
  test_check(2U, signed_round_trip(123456789L) == 123456789L);
  test_check(3U, signed_round_trip(-2147483647L) == -2147483647L);
  test_check(4U, unsigned_round_trip(0xfedcba98UL) == 0xfedcba98UL);
  test_check(5U, unsigned_round_trip(123456789UL) == 123456789UL);
  simulator_stop();
}
