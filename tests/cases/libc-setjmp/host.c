#include "test.h"
#include <setjmp.h>

static jmp_buf first_environment;
static jmp_buf second_environment;
static volatile int jump_phase;
static volatile unsigned int stack_checksum;

static void deepest_jump(int value)
{
  volatile unsigned int scratch[12];
  unsigned int index;

  for (index = 0U; index < 12U; ++index)
    scratch[index] = index + 0x1200U;
  stack_checksum = scratch[0] + scratch[11];
  longjmp(first_environment, value);
}

static void nested_jump(void)
{
  volatile unsigned int scratch[9];
  unsigned int index;

  for (index = 0U; index < 9U; ++index)
    scratch[index] = index;
  deepest_jump(42);
}

void main(void)
{
  int value;

  simulator_result = 0U;
  jump_phase = 0;
  stack_checksum = 0U;

  value = setjmp(first_environment);
  if (value == 0) {
    jump_phase = 1;
    nested_jump();
    test_check(1U, 0U);
  }
  test_check(2U, value == 42);
  test_check(3U, jump_phase == 1);
  test_check(4U, stack_checksum == 0x240bU);

  value = setjmp(second_environment);
  if (value == 0) {
    jump_phase = 2;
    longjmp(second_environment, 0);
    test_check(5U, 0U);
  }
  test_check(6U, value == 1);
  test_check(7U, jump_phase == 2);

  simulator_stop();
}
