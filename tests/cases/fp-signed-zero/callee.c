#include "test.h"

volatile test_f64 extended_result;

void extend_float(float value)
{
  extended_result.value = (double)value;
}

test_u16 extended_parameter_word(float value)
{
  test_f64 extended;
  extended.value = (double)value;
  return extended.words[0];
}

test_u16 double_parameter_word(double value)
{
  test_f64 copy;
  copy.value = value;
  return copy.words[0];
}
