#include "test.h"

static volatile test_i32 signed_a;
static volatile test_i32 signed_b;
static volatile test_i32 signed_result;
static volatile test_u32 unsigned_a;
static volatile test_u32 unsigned_b;
static volatile test_u32 unsigned_result;
static volatile test_u16 shift_count;

static test_i32 add_one(test_i32 value)
{
  return value + 1L;
}

void main(void)
{
  test_i32 (*function_pointer)(test_i32);

  simulator_result = 0U;
  signed_a = -123456L;
  signed_b = 321L;
  signed_result = signed_a * signed_b;
  test_check(1U, signed_result == -39629376L);
  signed_result = signed_a / signed_b;
  test_check(2U, signed_result == -384L);
  signed_result = signed_a % signed_b;
  test_check(3U, signed_result == -192L);

  unsigned_a = 0xfedcba98UL;
  unsigned_b = 0x1234UL;
  unsigned_result = unsigned_a / unsigned_b;
  test_check(4U, unsigned_result == 0x000e0042UL);
  unsigned_result = unsigned_a % unsigned_b;
  test_check(5U, unsigned_result == 0x00000930UL);

  shift_count = 7U;
  signed_a = 0x00123456L;
  signed_result = signed_a << shift_count;
  test_check(6U, signed_result == 0x091a2b00L);
  signed_a = -19088744L;
  signed_result = signed_a >> shift_count;
  test_check(7U, signed_result == -149131L);
  unsigned_a = 0xf1234567UL;
  unsigned_result = unsigned_a >> shift_count;
  test_check(8U, unsigned_result == 0x01e2468aUL);

  function_pointer = add_one;
  signed_result = function_pointer(41L);
  test_check(9U, signed_result == 42L);

  simulator_stop();
}
