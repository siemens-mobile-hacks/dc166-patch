#include "test.h"


static volatile test_u16 _near near_initialized = 0x1357U;
static volatile test_u32 _far far_initialized = 0x89abcdefUL;
static volatile test_u8 _huge huge_initialized[6] = {
  0x10U, 0x32U, 0x54U, 0x76U, 0x98U, 0xbaU
};
static volatile test_u16 _near near_zero;
static volatile test_u32 _far far_zero;
static volatile test_u8 _huge huge_zero[6];

void main(void)
{
  simulator_result = 0U;
  test_check(1U, near_initialized == 0x1357U);
  test_check(2U, far_initialized == 0x89abcdefUL);
  test_check(3U, huge_initialized[0] == 0x10U &&
                 huge_initialized[1] == 0x32U &&
                 huge_initialized[2] == 0x54U &&
                 huge_initialized[3] == 0x76U &&
                 huge_initialized[4] == 0x98U &&
                 huge_initialized[5] == 0xbaU);
  test_check(4U, near_zero == 0U);
  test_check(5U, far_zero == 0UL);
  test_check(6U, huge_zero[0] == 0U && huge_zero[5] == 0U);
  simulator_stop();
}
