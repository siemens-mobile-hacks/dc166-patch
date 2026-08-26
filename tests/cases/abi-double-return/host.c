#include "test.h"

extern test_u16 abi_check_tasking_double_return(test_u16 w3, test_u16 w2,
                                                test_u16 w1, test_u16 w0,
                                                test_u16 tail);
extern double _stackparm abi_r4_only_double_identity(double value,
                                                      test_u16 tail);

double _stackparm tasking_double_identity(double value, test_u16 tail)
{
  (void)tail;
  return value;
}

static void check_vector(test_u16 base, test_u16 w3, test_u16 w2,
                         test_u16 w1, test_u16 w0, test_u16 tail)
{
  test_f64 input;
  test_f64 returned;

  input.words[0] = w3;
  input.words[1] = w2;
  input.words[2] = w1;
  input.words[3] = w0;

  /* Assembly calls a TASKING callee and consumes only the public R4 result
     pointer.  This catches a callee that exposes the block only in R10. */
  test_check(base,
             abi_check_tasking_double_return(w3, w2, w1, w0, tail) == 0U);

  /* The assembly callee exposes the result only in R4 and points R10 at a
     poison block.  This independently verifies the TASKING caller side. */
  returned.value = abi_r4_only_double_identity(input.value, tail);
  test_check_f64((test_u16)(base + 1U), &returned, w3, w2, w1, w0);
}

void main(void)
{
  check_vector(1U, 0x0000U, 0x0000U, 0x0000U, 0x0000U, 0x0000U);
  check_vector(3U, 0x8000U, 0x0000U, 0x0000U, 0x0000U, 0x0001U);
  check_vector(5U, 0x3ff0U, 0x0000U, 0x0000U, 0x0000U, 0x1357U);
  check_vector(7U, 0xfff0U, 0x0000U, 0x0000U, 0x0000U, 0x8000U);
  check_vector(9U, 0x7ff8U, 0x1234U, 0x5678U, 0x9abcU, 0xffffU);
  check_vector(11U, 0x7ff0U, 0x0000U, 0x0000U, 0x0001U, 0x55aaU);
  check_vector(13U, 0x0000U, 0x0000U, 0x0000U, 0x0001U, 0xaa55U);
  check_vector(15U, 0xffffU, 0xffffU, 0xffffU, 0xffffU, 0x2468U);
  check_vector(17U, 0x0123U, 0x4567U, 0x89abU, 0xcdefU, 0x369cU);
  simulator_stop();
}
