#include "test.h"

volatile float f32_a[4];
volatile float f32_b[4];
volatile double f64_a[4];
volatile double f64_b[4];

extern float evaluate_float(unsigned int operation, unsigned int index);
extern double evaluate_double(unsigned int operation, unsigned int index);

static void set_f32(volatile float *value, test_u16 high, test_u16 low)
{
  volatile test_u16 *words = (volatile test_u16 *)value;
  words[0] = high;
  words[1] = low;
}

static void set_f64(volatile double *value, test_u16 w3, test_u16 w2,
                    test_u16 w1, test_u16 w0)
{
  volatile test_u16 *words = (volatile test_u16 *)value;
  words[0] = w3;
  words[1] = w2;
  words[2] = w1;
  words[3] = w0;
}

void main(void)
{
  test_f32 f32;
  test_f64 f64;

  simulator_result = 0U;

  set_f32(&f32_a[1], 0x3fc0U, 0x0000U);
  set_f32(&f32_b[1], 0x4000U, 0x0000U);
  f32.value = evaluate_float(0U, 1U);
  test_check_f32(1U, &f32, 0x3fc0U, 0x0000U);
  f32.value = evaluate_float(1U, 1U);
  test_check_f32(2U, &f32, 0x3fc0U, 0x0000U);
  set_f32(&f32_b[1], 0x4000U, 0x0000U);
  f32.value = evaluate_float(2U, 1U);
  test_check_f32(3U, &f32, 0x4060U, 0x0000U);
  set_f32(&f32_b[1], 0x4000U, 0x0000U);
  f32.value = evaluate_float(3U, 1U);
  test_check_f32(4U, &f32, 0x4040U, 0x0000U);
  set_f32(&f32_b[1], 0x4000U, 0x0000U);
  f32.value = evaluate_float(4U, 1U);
  test_check_f32(5U, &f32, 0x4060U, 0x0000U);
  f32.value = evaluate_float(5U, 1U);
  test_check_f32(6U, &f32, 0x4040U, 0x0000U);

  set_f64(&f64_a[1], 0x3ff8U, 0x0000U, 0x0000U, 0x0000U);
  set_f64(&f64_b[1], 0x4000U, 0x0000U, 0x0000U, 0x0000U);
  f64.value = evaluate_double(0U, 1U);
  test_check_f64(7U, &f64, 0x3ff8U, 0x0000U, 0x0000U, 0x0000U);
  f64.value = evaluate_double(1U, 1U);
  test_check_f64(8U, &f64, 0x3ff8U, 0x0000U, 0x0000U, 0x0000U);
  set_f64(&f64_b[1], 0x4000U, 0x0000U, 0x0000U, 0x0000U);
  f64.value = evaluate_double(2U, 1U);
  test_check_f64(9U, &f64, 0x400cU, 0x0000U, 0x0000U, 0x0000U);
  set_f64(&f64_b[1], 0x4000U, 0x0000U, 0x0000U, 0x0000U);
  f64.value = evaluate_double(3U, 1U);
  test_check_f64(10U, &f64, 0x4008U, 0x0000U, 0x0000U, 0x0000U);
  set_f64(&f64_b[1], 0x4000U, 0x0000U, 0x0000U, 0x0000U);
  f64.value = evaluate_double(4U, 1U);
  test_check_f64(11U, &f64, 0x400cU, 0x0000U, 0x0000U, 0x0000U);
  f64.value = evaluate_double(5U, 1U);
  test_check_f64(12U, &f64, 0x4008U, 0x0000U, 0x0000U, 0x0000U);

  simulator_stop();
}
