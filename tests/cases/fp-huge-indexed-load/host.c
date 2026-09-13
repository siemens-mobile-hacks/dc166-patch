#include "test.h"

extern volatile float indexed_values[];

static void check_indexed_value(test_u16 id, test_u16 index,
                                test_u16 high, test_u16 low)
{
  test_f32 value;
  value.value = indexed_values[index];
  test_check_f32(id, &value, high, low);
}

void main(void)
{
  check_indexed_value(1U, 0U, 0x3f80U, 0x0000U);
  check_indexed_value(2U, 1U, 0x8000U, 0x0000U);
  check_indexed_value(3U, 2U, 0x3fc0U, 0x0000U);
  check_indexed_value(4U, 3U, 0xc010U, 0x0000U);
  simulator_stop();
}
