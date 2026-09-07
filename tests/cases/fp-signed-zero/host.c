#include "test.h"

extern test_u16 extended_parameter_word(float value);
extern test_u16 double_parameter_word(double value);

static test_f32 f32_value;
static test_f64 f64_value;

void main(void)
{
  f32_value.value = -0.0f;
  test_check_f32(1U, &f32_value, 0x8000U, 0x0000U);

  f64_value.value = -0.0;
  test_check_f64(2U, &f64_value,
                 0x8000U, 0x0000U, 0x0000U, 0x0000U);

  test_check(3U, extended_parameter_word(-0.0f) == 0x8000U);
  test_check(4U, extended_parameter_word(0.0f) == 0x0000U);
  test_check(5U, double_parameter_word(-0.0) == 0x8000U);

  simulator_stop();
}
