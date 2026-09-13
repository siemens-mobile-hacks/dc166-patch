#include "test.h"
#include "vectors.inc"

typedef union { test_u32 all; test_u16 word[2]; } integer_words;
struct f32_integer { test_u32 bits; test_u32 expected; };
struct i32_f32 { test_i32 value; test_u32 expected; };
struct u32_f32 { test_u32 value; test_u32 expected; };
struct i16_f32 { test_i16 value; test_u32 expected; };
struct u16_f32 { test_u16 value; test_u32 expected; };

#define F32_INTEGER_ROW(id, pool, bits, expected) { bits, expected },
static const struct f32_integer f32_i32[] = { ABI_F32_TO_I32_VECTORS(F32_INTEGER_ROW) };
static const struct f32_integer f32_u32[] = { ABI_F32_TO_U32_VECTORS(F32_INTEGER_ROW) };
static const struct f32_integer f32_i16[] = { ABI_F32_TO_I16_VECTORS(F32_INTEGER_ROW) };
static const struct f32_integer f32_u16[] = { ABI_F32_TO_U16_VECTORS(F32_INTEGER_ROW) };
#undef F32_INTEGER_ROW
#define I32_ROW(id, value, expected) { value, expected },
static const struct i32_f32 i32_f32[] = { ABI_I32_TO_F32_VECTORS(I32_ROW) };
#undef I32_ROW
#define U32_ROW(id, value, expected) { value, expected },
static const struct u32_f32 u32_f32[] = { ABI_U32_TO_F32_VECTORS(U32_ROW) };
#undef U32_ROW
#define I16_ROW(id, value, expected) { value, expected },
static const struct i16_f32 i16_f32[] = { ABI_I16_TO_F32_VECTORS(I16_ROW) };
#undef I16_ROW
#define U16_ROW(id, value, expected) { value, expected },
static const struct u16_f32 u16_f32[] = { ABI_U16_TO_F32_VECTORS(U16_ROW) };
#undef U16_ROW

extern test_i32 convert_f32_i32(float);
extern test_u32 convert_f32_u32(float);
extern test_i16 convert_f32_i16(float);
extern test_u16 convert_f32_u16(float);
extern float convert_i32_f32(test_i32);
extern float convert_u32_f32(test_u32);
extern float convert_i16_f32(test_i16);
extern float convert_u16_f32(test_u16);

#define COUNT(a) (sizeof(a) / sizeof((a)[0]))

static float from_bits(test_u32 bits)
{
  integer_words integer;
  test_f32 value;
  integer.all = bits;
  value.words[0] = integer.word[1];
  value.words[1] = integer.word[0];
  return value.value;
}

static test_u32 to_bits(float input)
{
  integer_words integer;
  test_f32 value;
  value.value = input;
  integer.word[0] = value.words[1];
  integer.word[1] = value.words[0];
  return integer.all;
}

void main(void)
{
  test_u16 id = 0U;
  test_u16 index;
  simulator_result = 0U;
  for (index = 0U; index != COUNT(f32_i32); ++index)
    test_check(++id, (test_u32)convert_f32_i32(from_bits(f32_i32[index].bits)) == f32_i32[index].expected);
  for (index = 0U; index != COUNT(f32_u32); ++index)
    test_check(++id, convert_f32_u32(from_bits(f32_u32[index].bits)) == f32_u32[index].expected);
  for (index = 0U; index != COUNT(f32_i16); ++index)
    test_check(++id, (test_u32)(test_i32)convert_f32_i16(from_bits(f32_i16[index].bits)) == f32_i16[index].expected);
  for (index = 0U; index != COUNT(f32_u16); ++index)
    test_check(++id, (test_u32)convert_f32_u16(from_bits(f32_u16[index].bits)) == f32_u16[index].expected);
  for (index = 0U; index != COUNT(i32_f32); ++index)
    test_check(++id, to_bits(convert_i32_f32(i32_f32[index].value)) == i32_f32[index].expected);
  for (index = 0U; index != COUNT(u32_f32); ++index)
    test_check(++id, to_bits(convert_u32_f32(u32_f32[index].value)) == u32_f32[index].expected);
  for (index = 0U; index != COUNT(i16_f32); ++index)
    test_check(++id, to_bits(convert_i16_f32(i16_f32[index].value)) == i16_f32[index].expected);
  for (index = 0U; index != COUNT(u16_f32); ++index)
    test_check(++id, to_bits(convert_u16_f32(u16_f32[index].value)) == u16_f32[index].expected);
  simulator_stop();
}
