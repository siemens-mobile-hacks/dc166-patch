#include "test.h"
#include "vectors.inc"

struct f64_integer { test_u16 words[4]; test_u32 expected; };
struct i32_f64 { test_i32 value; test_u32 hi; test_u32 lo; };
struct u32_f64 { test_u32 value; test_u32 hi; test_u32 lo; };
struct i16_f64 { test_i16 value; test_u32 hi; test_u32 lo; };
struct u16_f64 { test_u16 value; test_u32 hi; test_u32 lo; };

#define F64_INTEGER_ROW(id, w3, w2, w1, w0, expected) { { w3, w2, w1, w0 }, expected },
static const struct f64_integer f64_i32[] = { ABI_F64_TO_I32_VECTORS(F64_INTEGER_ROW) };
static const struct f64_integer f64_u32[] = { ABI_F64_TO_U32_VECTORS(F64_INTEGER_ROW) };
static const struct f64_integer f64_i16[] = { ABI_F64_TO_I16_VECTORS(F64_INTEGER_ROW) };
static const struct f64_integer f64_u16[] = { ABI_F64_TO_U16_VECTORS(F64_INTEGER_ROW) };
#undef F64_INTEGER_ROW
#define I32_ROW(id, value, hi, lo) { value, hi, lo },
static const struct i32_f64 i32_f64[] = { ABI_I32_TO_F64_VECTORS(I32_ROW) };
#undef I32_ROW
#define U32_ROW(id, value, hi, lo) { value, hi, lo },
static const struct u32_f64 u32_f64[] = { ABI_U32_TO_F64_VECTORS(U32_ROW) };
#undef U32_ROW
#define I16_ROW(id, value, hi, lo) { value, hi, lo },
static const struct i16_f64 i16_f64[] = { ABI_I16_TO_F64_VECTORS(I16_ROW) };
#undef I16_ROW
#define U16_ROW(id, value, hi, lo) { value, hi, lo },
static const struct u16_f64 u16_f64[] = { ABI_U16_TO_F64_VECTORS(U16_ROW) };
#undef U16_ROW

extern test_i32 convert_f64_i32(double);
extern test_u32 convert_f64_u32(double);
extern test_i16 convert_f64_i16(double);
extern test_u16 convert_f64_u16(double);
extern double convert_i32_f64(test_i32);
extern double convert_u32_f64(test_u32);
extern double convert_i16_f64(test_i16);
extern double convert_u16_f64(test_u16);

#define COUNT(a) (sizeof(a) / sizeof((a)[0]))

static double from_words(const test_u16 words[4])
{
  test_f64 value;
  value.words[0] = words[0]; value.words[1] = words[1];
  value.words[2] = words[2]; value.words[3] = words[3];
  return value.value;
}

static void check_result(test_u16 id, double input, test_u32 hi, test_u32 lo)
{
  test_f64 value;
  test_u16 h3 = (test_u16)(hi >> 16);
  test_u16 h2 = (test_u16)hi;
  test_u16 h1 = (test_u16)(lo >> 16);
  test_u16 h0 = (test_u16)lo;
  value.value = input;
  test_check_f64(id, &value, h3, h2, h1, h0);
}

void main(void)
{
  test_u16 id = 0U;
  test_u16 index;
  simulator_result = 0U;
  for (index = 0U; index != COUNT(f64_i32); ++index)
    test_check(++id, (test_u32)convert_f64_i32(from_words(f64_i32[index].words)) == f64_i32[index].expected);
  for (index = 0U; index != COUNT(f64_u32); ++index)
    test_check(++id, convert_f64_u32(from_words(f64_u32[index].words)) == f64_u32[index].expected);
  for (index = 0U; index != COUNT(f64_i16); ++index)
    test_check(++id, (test_u32)(test_i32)convert_f64_i16(from_words(f64_i16[index].words)) == f64_i16[index].expected);
  for (index = 0U; index != COUNT(f64_u16); ++index)
    test_check(++id, (test_u32)convert_f64_u16(from_words(f64_u16[index].words)) == f64_u16[index].expected);
  for (index = 0U; index != COUNT(i32_f64); ++index)
    check_result(++id, convert_i32_f64(i32_f64[index].value), i32_f64[index].hi, i32_f64[index].lo);
  for (index = 0U; index != COUNT(u32_f64); ++index)
    check_result(++id, convert_u32_f64(u32_f64[index].value), u32_f64[index].hi, u32_f64[index].lo);
  for (index = 0U; index != COUNT(i16_f64); ++index)
    check_result(++id, convert_i16_f64(i16_f64[index].value), i16_f64[index].hi, i16_f64[index].lo);
  for (index = 0U; index != COUNT(u16_f64); ++index)
    check_result(++id, convert_u16_f64(u16_f64[index].value), u16_f64[index].hi, u16_f64[index].lo);
  simulator_stop();
}
