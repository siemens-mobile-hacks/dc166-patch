#include "test.h"

static volatile test_f32 f32_value;
static volatile test_f64 f64_value;
static volatile test_i16 i16_value;
static volatile test_u16 u16_value;
static volatile test_i32 i32_value;
static volatile test_u32 u32_value;

static void set_f32(test_u16 high, test_u16 low)
{
  f32_value.words[0] = high;
  f32_value.words[1] = low;
}

static void set_f64(test_u16 w3, test_u16 w2, test_u16 w1, test_u16 w0)
{
  f64_value.words[0] = w3;
  f64_value.words[1] = w2;
  f64_value.words[2] = w1;
  f64_value.words[3] = w0;
}

void main(void)
{
  simulator_result = 0U;

  i16_value = -123;
  f32_value.value = (float)i16_value;
  test_check_f32(1U, (test_f32 *)&f32_value, 0xc2f6U, 0x0000U);
  u16_value = 60000U;
  f32_value.value = (float)u16_value;
  test_check_f32(2U, (test_f32 *)&f32_value, 0x476aU, 0x6000U);
  i32_value = -65536L;
  f32_value.value = (float)i32_value;
  test_check_f32(3U, (test_f32 *)&f32_value, 0xc780U, 0x0000U);
  u32_value = 65536UL;
  f32_value.value = (float)u32_value;
  test_check_f32(4U, (test_f32 *)&f32_value, 0x4780U, 0x0000U);

  set_f32(0xc2f7U, 0x0000U);
  i16_value = (test_i16)f32_value.value;
  test_check(5U, i16_value == -123);
  set_f32(0x476aU, 0x6000U);
  u16_value = (test_u16)f32_value.value;
  test_check(6U, u16_value == 60000U);
  set_f32(0xc780U, 0x0040U);
  i32_value = (test_i32)f32_value.value;
  test_check(7U, i32_value == -65536L);
  set_f32(0x4780U, 0x0020U);
  u32_value = (test_u32)f32_value.value;
  test_check(8U, u32_value == 65536UL);

  i16_value = -123;
  f64_value.value = (double)i16_value;
  test_check_f64(9U, (test_f64 *)&f64_value,
                 0xc05eU, 0xc000U, 0x0000U, 0x0000U);
  u16_value = 60000U;
  f64_value.value = (double)u16_value;
  test_check_f64(10U, (test_f64 *)&f64_value,
                 0x40edU, 0x4c00U, 0x0000U, 0x0000U);
  i32_value = -65536L;
  f64_value.value = (double)i32_value;
  test_check_f64(11U, (test_f64 *)&f64_value,
                 0xc0f0U, 0x0000U, 0x0000U, 0x0000U);
  u32_value = 65536UL;
  f64_value.value = (double)u32_value;
  test_check_f64(12U, (test_f64 *)&f64_value,
                 0x40f0U, 0x0000U, 0x0000U, 0x0000U);

  set_f64(0xc05eU, 0xf000U, 0x0000U, 0x0000U);
  i16_value = (test_i16)f64_value.value;
  test_check(13U, i16_value == -123);
  set_f64(0x40edU, 0x4c00U, 0x0000U, 0x0000U);
  u16_value = (test_u16)f64_value.value;
  test_check(14U, u16_value == 60000U);
  set_f64(0xc0f0U, 0x0000U, 0x8000U, 0x0000U);
  i32_value = (test_i32)f64_value.value;
  test_check(15U, i32_value == -65536L);
  set_f64(0x40f0U, 0x0000U, 0x4000U, 0x0000U);
  u32_value = (test_u32)f64_value.value;
  test_check(16U, u32_value == 65536UL);

  set_f32(0x3fc0U, 0x0000U);
  f64_value.value = (double)f32_value.value;
  test_check_f64(17U, (test_f64 *)&f64_value,
                 0x3ff8U, 0x0000U, 0x0000U, 0x0000U);
  set_f64(0x4002U, 0x0000U, 0x0000U, 0x0000U);
  f32_value.value = (float)f64_value.value;
  test_check_f32(18U, (test_f32 *)&f32_value, 0x4010U, 0x0000U);

  simulator_stop();
}
