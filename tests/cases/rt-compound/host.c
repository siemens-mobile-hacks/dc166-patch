#include "test.h"

static volatile test_i32 _near near_signed;
static volatile test_u32 _near near_unsigned;
static volatile test_i32 _far far_signed;
static volatile test_u32 _far far_unsigned;
static volatile test_i32 _huge huge_signed;
static volatile test_u32 _huge huge_unsigned;
static volatile test_i32 signed_operand;
static volatile test_u32 unsigned_operand;
static volatile test_u16 shift_operand;

#define CHECK_COMPOUND(SIGNED_VALUE, UNSIGNED_VALUE, ID)                     \
  do {                                                                       \
    signed_operand = 17L;                                                    \
    (SIGNED_VALUE) = -1234L;                                                 \
    (SIGNED_VALUE) *= signed_operand;                                        \
    test_check((ID), (SIGNED_VALUE) == -20978L);                             \
    signed_operand = 67L;                                                    \
    (SIGNED_VALUE) = -12345L;                                                \
    (SIGNED_VALUE) /= signed_operand;                                        \
    test_check((ID) + 1U, (SIGNED_VALUE) == -184L);                          \
    (SIGNED_VALUE) = -12345L;                                                \
    (SIGNED_VALUE) %= signed_operand;                                        \
    test_check((ID) + 2U, (SIGNED_VALUE) == -17L);                           \
    unsigned_operand = 0x1234UL;                                             \
    (UNSIGNED_VALUE) = 0xfedcba98UL;                                         \
    (UNSIGNED_VALUE) /= unsigned_operand;                                    \
    test_check((ID) + 3U, (UNSIGNED_VALUE) == 0x000e0042UL);                 \
    (UNSIGNED_VALUE) = 0xfedcba98UL;                                         \
    (UNSIGNED_VALUE) %= unsigned_operand;                                    \
    test_check((ID) + 4U, (UNSIGNED_VALUE) == 0x00000930UL);                 \
    shift_operand = 7U;                                                      \
    (SIGNED_VALUE) = 0x00123456L;                                            \
    (SIGNED_VALUE) <<= shift_operand;                                        \
    test_check((ID) + 5U, (SIGNED_VALUE) == 0x091a2b00L);                    \
    (SIGNED_VALUE) = -19088744L;                                             \
    (SIGNED_VALUE) >>= shift_operand;                                        \
    test_check((ID) + 6U, (SIGNED_VALUE) == -149131L);                       \
    (UNSIGNED_VALUE) = 0xf1234567UL;                                         \
    (UNSIGNED_VALUE) >>= shift_operand;                                      \
    test_check((ID) + 7U, (UNSIGNED_VALUE) == 0x01e2468aUL);                 \
  } while (0)

void main(void)
{
  simulator_result = 0U;
  CHECK_COMPOUND(near_signed, near_unsigned, 1U);
  CHECK_COMPOUND(far_signed, far_unsigned, 9U);
  CHECK_COMPOUND(huge_signed, huge_unsigned, 17U);
  simulator_stop();
}
