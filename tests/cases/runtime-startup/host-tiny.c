#include "test.h"

static volatile test_u16 initialized_word = 0x1357U;
static volatile test_u8 initialized_bytes[6] = {
  0x10U, 0x32U, 0x54U, 0x76U, 0x98U, 0xbaU
};
static volatile test_u16 zero_word;
static volatile test_u8 zero_bytes[6];

void main(void)
{
  simulator_result = 0U;
  test_check(1U, initialized_word == 0x1357U);
  test_check(2U, initialized_bytes[0] == 0x10U &&
                 initialized_bytes[1] == 0x32U &&
                 initialized_bytes[2] == 0x54U &&
                 initialized_bytes[3] == 0x76U &&
                 initialized_bytes[4] == 0x98U &&
                 initialized_bytes[5] == 0xbaU);
  test_check(3U, zero_word == 0U);
  test_check(4U, zero_bytes[0] == 0U && zero_bytes[5] == 0U);
  simulator_stop();
}
