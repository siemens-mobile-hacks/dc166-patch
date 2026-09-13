#include "test.h"

extern float select_f32(float value, unsigned int select_value);
extern double select_f64(double value, unsigned int select_value);

void main(void)
{
  test_f32 f32_input;
  test_f32 f32_output;
  test_f64 f64_input;
  test_f64 f64_output;

  simulator_result = 0U;

  f32_input.words[0] = 0x8000U;
  f32_input.words[1] = 0x0000U;
  f32_output.value = select_f32(f32_input.value, 0x5a5aU);
  test_check_f32(1U, &f32_output, 0x8000U, 0x0000U);
  f32_output.value = select_f32(f32_input.value, 0U);
  test_check_f32(2U, &f32_output, 0x4640U, 0xe400U);

  f64_input.words[0] = 0x8000U;
  f64_input.words[1] = 0x0000U;
  f64_input.words[2] = 0x0000U;
  f64_input.words[3] = 0x0000U;
  f64_output.value = select_f64(f64_input.value, 0x5a5aU);
  test_check_f64(3U, &f64_output,
                 0x8000U, 0x0000U, 0x0000U, 0x0000U);
  f64_output.value = select_f64(f64_input.value, 0U);
  test_check_f64(4U, &f64_output,
                 0x40c8U, 0x1c80U, 0x0000U, 0x0000U);

  simulator_stop();
}
