#include "test.h"
#include "vectors.inc"

struct arithmetic_vector {
  test_u16 operation;
  test_u16 lhs[4];
  test_u16 rhs[4];
  test_u16 expected[4];
  test_u16 any_nan_allowed;
};

#define ARITHMETIC_ROW(id, operation, name, l3, l2, l1, l0, r3, r2, r1, r0, e3, e2, e1, e0, nan_ok) \
  { operation, { l3, l2, l1, l0 }, { r3, r2, r1, r0 }, \
    { e3, e2, e1, e0 }, nan_ok },
static const struct arithmetic_vector vectors[] = {
  ABI_FLOAT64_VECTORS(ARITHMETIC_ROW)
};
#undef ARITHMETIC_ROW

static volatile test_f64 left_value;
static volatile test_f64 right_value;
static volatile test_f64 actual_value;

static void set_value(volatile test_f64 *value, const test_u16 words[4])
{
  value->words[0] = words[0];
  value->words[1] = words[1];
  value->words[2] = words[2];
  value->words[3] = words[3];
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
  for (index = 0U; index != ABI_FLOAT64_VECTOR_COUNT; ++index) {
    const struct arithmetic_vector *vector = &vectors[index];
    test_u16 exact;
    test_u16 is_nan;

    set_value(&left_value, vector->lhs);
    set_value(&right_value, vector->rhs);
    evaluate(vector->operation);
    exact = actual_value.words[0] == vector->expected[0] &&
            actual_value.words[1] == vector->expected[1] &&
            actual_value.words[2] == vector->expected[2] &&
            actual_value.words[3] == vector->expected[3];
    is_nan = (actual_value.words[0] & 0x7ff0U) == 0x7ff0U &&
             ((actual_value.words[0] & 0x000fU) |
              actual_value.words[1] | actual_value.words[2] |
              actual_value.words[3]) != 0U;
    test_check(index + 1U,
               exact || (vector->any_nan_allowed != 0U && is_nan));
  }
  simulator_stop();
}
