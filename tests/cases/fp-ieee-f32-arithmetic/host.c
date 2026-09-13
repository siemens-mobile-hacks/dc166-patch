#include "test.h"
#include "vectors.inc"

struct arithmetic_vector {
  test_u16 operation;
  test_u32 lhs;
  test_u32 rhs;
  test_u32 expected;
  test_u16 any_nan_allowed;
};

#define ARITHMETIC_ROW(id, operation, name, lhs, rhs, expected, nan_ok) \
  { operation, lhs, rhs, expected, nan_ok },
static const struct arithmetic_vector vectors[] = {
  ABI_FLOAT32_VECTORS(ARITHMETIC_ROW)
};
#undef ARITHMETIC_ROW

typedef union {
  test_u32 all;
  test_u16 word[2];
} integer_words;

static volatile test_f32 left_value;
static volatile test_f32 right_value;
static volatile test_f32 actual_value;

static void set_value(volatile test_f32 *value, test_u32 bits)
{
  integer_words words;
  words.all = bits;
  value->words[0] = words.word[1];
  value->words[1] = words.word[0];
}

static test_u32 get_value(volatile test_f32 *value)
{
  integer_words words;
  words.word[0] = value->words[1];
  words.word[1] = value->words[0];
  return words.all;
}

static void evaluate(test_u16 operation)
{
  if (operation == 0U)
    actual_value.value = left_value.value + right_value.value;
  else if (operation == 1U)
    actual_value.value = left_value.value - right_value.value;
  else if (operation == 2U)
    actual_value.value = left_value.value * right_value.value;
  else
    actual_value.value = left_value.value / right_value.value;
}

void main(void)
{
  test_u16 index;

  simulator_result = 0U;
  for (index = 0U; index != ABI_FLOAT32_VECTOR_COUNT; ++index) {
    const struct arithmetic_vector *vector = &vectors[index];
    test_u32 actual;
    test_u16 is_nan;

    set_value(&left_value, vector->lhs);
    set_value(&right_value, vector->rhs);
    evaluate(vector->operation);
    actual = get_value(&actual_value);
    is_nan = (actual & 0x7f800000UL) == 0x7f800000UL &&
             (actual & 0x007fffffUL) != 0UL;
    test_check(index + 1U,
               actual == vector->expected ||
               (vector->any_nan_allowed != 0U && is_nan));
  }
  simulator_stop();
}
