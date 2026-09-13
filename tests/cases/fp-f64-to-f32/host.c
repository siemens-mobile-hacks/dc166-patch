#include "test.h"

extern float convert_f64_to_f32(double value);

static void check_conversion(test_u16 id,
                             test_u16 w3, test_u16 w2,
                             test_u16 w1, test_u16 w0,
                             test_u16 expected_high,
                             test_u16 expected_low)
{
  test_f64 input;
  test_f32 output;

  input.words[0] = w3;
  input.words[1] = w2;
  input.words[2] = w1;
  input.words[3] = w0;
  output.value = convert_f64_to_f32(input.value);
  test_check_f32(id, &output, expected_high, expected_low);
}

void main(void)
{
  simulator_result = 0U;

  check_conversion(1U, 0x0000U, 0x0000U, 0x0000U, 0x0000U,
                    0x0000U, 0x0000U);
  check_conversion(2U, 0x8000U, 0x0000U, 0x0000U, 0x0000U,
                    0x8000U, 0x0000U);
  check_conversion(3U, 0x0010U, 0x0000U, 0x0000U, 0x0000U,
                    0x0000U, 0x0000U);

  check_conversion(4U, 0x3698U, 0x0000U, 0x0000U, 0x0000U,
                    0x0000U, 0x0001U);
  check_conversion(5U, 0x36a0U, 0x0000U, 0x0000U, 0x0000U,
                    0x0000U, 0x0001U);
  check_conversion(6U, 0x380fU, 0xffffU, 0xc000U, 0x0000U,
                    0x007fU, 0xffffU);
  check_conversion(7U, 0x380fU, 0xffffU, 0xe000U, 0x0000U,
                    0x0080U, 0x0000U);
  check_conversion(8U, 0x3800U, 0x0000U, 0x2000U, 0x0000U,
                    0x0040U, 0x0000U);
  check_conversion(9U, 0x3800U, 0x0000U, 0x2000U, 0x0001U,
                    0x0040U, 0x0001U);

  check_conversion(10U, 0x3ff0U, 0x0000U, 0x1000U, 0x0000U,
                    0x3f80U, 0x0000U);
  check_conversion(11U, 0x3ff0U, 0x0000U, 0x1000U, 0x0001U,
                    0x3f80U, 0x0001U);
  check_conversion(12U, 0x3ff0U, 0x0000U, 0x3000U, 0x0000U,
                    0x3f80U, 0x0002U);
  check_conversion(13U, 0xbff0U, 0x0000U, 0x1000U, 0x0000U,
                    0xbf80U, 0x0000U);

  check_conversion(14U, 0x47efU, 0xffffU, 0xe000U, 0x0000U,
                    0x7f7fU, 0xffffU);
  check_conversion(15U, 0x47efU, 0xffffU, 0xf000U, 0x0000U,
                    0x7f80U, 0x0000U);
  check_conversion(16U, 0x7ff0U, 0x0000U, 0x0000U, 0x0000U,
                    0x7f80U, 0x0000U);

  check_conversion(17U, 0x7ff8U, 0x1234U, 0x5678U, 0x9abcU,
                    0x7fc0U, 0x91a2U);
  check_conversion(18U, 0x7ff0U, 0x0000U, 0x0000U, 0x0001U,
                    0x7fc0U, 0x0000U);
  check_conversion(19U, 0xfff9U, 0x2345U, 0x6789U, 0xabcdU,
                    0xffc9U, 0x1a2bU);

  simulator_stop();
}
