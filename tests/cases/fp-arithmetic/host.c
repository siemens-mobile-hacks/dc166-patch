#include "test.h"

static volatile test_f32 f32_a;
static volatile test_f32 f32_b;
static volatile test_f32 f32_result;
static volatile test_f64 f64_a;
static volatile test_f64 f64_b;
static volatile test_f64 f64_result;
static volatile test_i16 compare_result;

static void set_f32(volatile test_f32 *value, test_u16 high, test_u16 low)
{
  value->words[0] = high;
  value->words[1] = low;
}

static void set_f64(volatile test_f64 *value, test_u16 w3, test_u16 w2,
                    test_u16 w1, test_u16 w0)
{
  value->words[0] = w3;
  value->words[1] = w2;
  value->words[2] = w1;
  value->words[3] = w0;
}

static void test_float(void)
{
  set_f32(&f32_a, 0x3fc0U, 0x0000U);
  set_f32(&f32_b, 0x4010U, 0x0000U);

  f32_result.value = f32_a.value + f32_b.value;
  test_check_f32(1U, (test_f32 *)&f32_result, 0x4070U, 0x0000U);
  f32_result.value = f32_a.value - f32_b.value;
  test_check_f32(2U, (test_f32 *)&f32_result, 0xbf40U, 0x0000U);
  f32_result.value = f32_a.value * f32_b.value;
  test_check_f32(3U, (test_f32 *)&f32_result, 0x4058U, 0x0000U);
  f32_result.value = f32_a.value / f32_b.value;
  test_check_f32(4U, (test_f32 *)&f32_result, 0x3f2aU, 0xaaabU);

  compare_result = f32_a.value < f32_b.value;
  test_check(5U, compare_result != 0);
  compare_result = f32_a.value == f32_b.value;
  test_check(6U, compare_result == 0);
  compare_result = f32_a.value >= f32_b.value;
  test_check(7U, compare_result == 0);

  set_f32(&f32_a, 0x3f80U, 0x0000U);
  set_f32(&f32_b, 0x0000U, 0x0000U);
  f32_result.value = f32_a.value / f32_b.value;
}

static void test_double(void)
{
  set_f64(&f64_a, 0x3ff8U, 0x0000U, 0x0000U, 0x0000U);
  set_f64(&f64_b, 0x4002U, 0x0000U, 0x0000U, 0x0000U);

  f64_result.value = f64_a.value + f64_b.value;
  test_check_f64(9U, (test_f64 *)&f64_result,
                 0x400eU, 0x0000U, 0x0000U, 0x0000U);
  f64_result.value = f64_a.value - f64_b.value;
  test_check_f64(10U, (test_f64 *)&f64_result,
                 0xbfe8U, 0x0000U, 0x0000U, 0x0000U);
  f64_result.value = f64_a.value * f64_b.value;
  test_check_f64(11U, (test_f64 *)&f64_result,
                 0x400bU, 0x0000U, 0x0000U, 0x0000U);
  f64_result.value = f64_a.value / f64_b.value;
  test_check_f64(12U, (test_f64 *)&f64_result,
                 0x3fe5U, 0x5555U, 0x5555U, 0x5555U);
  f64_result.value = -f64_a.value;
  test_check_f64(13U, (test_f64 *)&f64_result,
                 0xbff8U, 0x0000U, 0x0000U, 0x0000U);

  compare_result = f64_a.value < f64_b.value;
  test_check(14U, compare_result != 0);
  compare_result = f64_a.value == f64_b.value;
  test_check(15U, compare_result == 0);
  compare_result = f64_a.value >= f64_b.value;
  test_check(16U, compare_result == 0);

  set_f64(&f64_a, 0x3ff0U, 0x0000U, 0x0000U, 0x0000U);
  set_f64(&f64_b, 0x0000U, 0x0000U, 0x0000U, 0x0000U);
  f64_result.value = f64_a.value / f64_b.value;
}

void main(void)
{
  simulator_result = 0U;
  test_float();
  test_double();
  simulator_stop();
}
