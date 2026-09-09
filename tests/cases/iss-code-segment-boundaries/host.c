#include "test.h"

extern test_u16 test_boundary_jmpr(void);
extern test_u16 test_boundary_jmpa(void);
extern test_u16 test_boundary_jmpi(void);
extern test_u16 test_boundary_jb(void);

void main(void)
{
  simulator_result = 0U;
  test_check(1U, test_boundary_jmpr() == 1U);
  test_check(2U, test_boundary_jmpa() == 1U);
  test_check(3U, test_boundary_jmpi() == 1U);
  test_check(4U, test_boundary_jb() == 1U);
  simulator_stop();
}
