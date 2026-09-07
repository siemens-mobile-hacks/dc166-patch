#include "test.h"

extern test_u16 extended_parameter_word(float value);
extern test_u16 double_parameter_word(double value);
extern void extend_float(float value);
extern volatile test_f64 extended_result;

static test_f32 f32_value;
static test_f64 f64_value;

static void check_extension(test_u16 id, test_u16 f1, test_u16 f0,
                            test_u16 d3, test_u16 d2,
                            test_u16 d1, test_u16 d0)
{
  f32_value.words[0] = f1;
  f32_value.words[1] = f0;
  extend_float(f32_value.value);
  test_check_f64(id, (test_f64 *)&extended_result, d3, d2, d1, d0);
}

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

  check_extension(6U, 0x0000U, 0x0000U,
                   0x0000U, 0x0000U, 0x0000U, 0x0000U);
  check_extension(7U, 0x8000U, 0x0000U,
                   0x8000U, 0x0000U, 0x0000U, 0x0000U);
  check_extension(8U, 0x0000U, 0x0001U,
                   0x36a0U, 0x0000U, 0x0000U, 0x0000U);
  check_extension(9U, 0x007fU, 0xffffU,
                   0x380fU, 0xffffU, 0xc000U, 0x0000U);
  check_extension(10U, 0x807fU, 0xffffU,
                   0xb80fU, 0xffffU, 0xc000U, 0x0000U);
  check_extension(11U, 0x0080U, 0x0000U,
                   0x3810U, 0x0000U, 0x0000U, 0x0000U);
  check_extension(12U, 0x3f80U, 0x0000U,
                   0x3ff0U, 0x0000U, 0x0000U, 0x0000U);
  check_extension(13U, 0xbfc0U, 0x0000U,
                   0xbff8U, 0x0000U, 0x0000U, 0x0000U);
  check_extension(14U, 0x7f7fU, 0xffffU,
                   0x47efU, 0xffffU, 0xe000U, 0x0000U);
  check_extension(15U, 0x7f80U, 0x0000U,
                   0x7ff0U, 0x0000U, 0x0000U, 0x0000U);
  check_extension(16U, 0xff80U, 0x0000U,
                   0xfff0U, 0x0000U, 0x0000U, 0x0000U);
  check_extension(17U, 0x7fc1U, 0x2345U,
                   0x7ff8U, 0x2468U, 0xa000U, 0x0000U);

  simulator_stop();
}
