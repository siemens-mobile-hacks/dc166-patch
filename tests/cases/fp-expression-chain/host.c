#include "test.h"

static volatile test_f64 result;

double expression_mul_add(double a, double b, double c)
{
  return a * b + c;
}

double expression_add_mul(double a, double b, double c)
{
  return a + b * c;
}

double expression_add_then_mul(double a, double b, double c)
{
  return (a + b) * c;
}

double expression_mul_by_add(double a, double b, double c)
{
  return a * (b + c);
}

double expression_two_products(double a, double b, double c, double d)
{
  return a * b + c * d;
}

static void check_result(test_u16 id, double value,
                         test_u16 w3, test_u16 w2,
                         test_u16 w1, test_u16 w0)
{
  result.value = value;
  test_check_f64(id, (test_f64 *)&result, w3, w2, w1, w0);
}

void main(void)
{
  simulator_result = 0U;

  check_result(1U, expression_mul_add(3.0, 4.0, 3.0),
               0x402eU, 0x0000U, 0x0000U, 0x0000U);
  check_result(2U, expression_mul_add(5.0, 7.0, 5.0),
               0x4044U, 0x0000U, 0x0000U, 0x0000U);
  check_result(3U, expression_add_mul(2.0, 3.0, 4.0),
               0x402cU, 0x0000U, 0x0000U, 0x0000U);
  check_result(4U, expression_add_then_mul(2.0, 3.0, 4.0),
               0x4034U, 0x0000U, 0x0000U, 0x0000U);
  check_result(5U, expression_mul_by_add(2.0, 3.0, 4.0),
               0x402cU, 0x0000U, 0x0000U, 0x0000U);
  check_result(6U, expression_two_products(2.0, 3.0, 4.0, 5.0),
               0x403aU, 0x0000U, 0x0000U, 0x0000U);

  simulator_stop();
}
