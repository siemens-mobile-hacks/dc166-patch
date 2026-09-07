#include "test.h"

_packed struct packed3 {
  test_u8 first;
  test_u16 second;
};

extern test_u16 fixed_packed_call(test_u16 fixed,
                                  _packed struct packed3 value,
                                  test_u16 tail);
extern test_u16 variadic_packed_call(test_u16 fixed, ...);

static void check_vector(test_u16 id, test_u16 seed)
{
  _packed struct packed3 value;
  test_u16 fixed = 0x1357U;
  test_u16 tail = (test_u16)(seed ^ 0xcafeU);
  test_u16 expected;

  value.first = (test_u8)(seed + 0x67U);
  value.second = (test_u16)(seed ^ 0x8888U);
  expected = (test_u16)(fixed * 3U + (test_u16)value.first * 19U +
                        value.second * 23U + tail * 29U);

  test_check(id, fixed_packed_call(fixed, value, tail) == expected);
  test_check((test_u16)(id + 1U),
             variadic_packed_call(fixed, value, tail) == expected);
}

void main(void)
{
  check_vector(1U, 0x0000U);
  check_vector(3U, 0x0001U);
  check_vector(5U, 0x005aU);
  check_vector(7U, 0x00a5U);
  check_vector(9U, 0xffffU);
  check_vector(11U, 0x8000U);
  simulator_stop();
}
