#include "test.h"

static volatile test_i32 signed_value;
static volatile test_u32 unsigned_value;
static volatile test_i32 signed_operand;
static volatile test_u32 unsigned_operand;
static volatile test_u16 shift_operand;

void main(void)
{
  simulator_result = 0U;
  signed_operand = 17L;
  signed_value = -1234L;
  signed_value *= signed_operand;
  test_check(1U, signed_value == -20978L);
  signed_operand = 67L;
  signed_value = -12345L;
  signed_value /= signed_operand;
  test_check(2U, signed_value == -184L);
  signed_value = -12345L;
  signed_value %= signed_operand;
  test_check(3U, signed_value == -17L);
  unsigned_operand = 0x1234UL;
  unsigned_value = 0xfedcba98UL;
  unsigned_value /= unsigned_operand;
  test_check(4U, unsigned_value == 0x000e0042UL);
  unsigned_value = 0xfedcba98UL;
  unsigned_value %= unsigned_operand;
  test_check(5U, unsigned_value == 0x00000930UL);
  shift_operand = 7U;
  signed_value = 0x00123456L;
  signed_value <<= shift_operand;
  test_check(6U, signed_value == 0x091a2b00L);
  signed_value = -19088744L;
  signed_value >>= shift_operand;
  test_check(7U, signed_value == -149131L);
  unsigned_value = 0xf1234567UL;
  unsigned_value >>= shift_operand;
  test_check(8U, unsigned_value == 0x01e2468aUL);
  simulator_stop();
}
