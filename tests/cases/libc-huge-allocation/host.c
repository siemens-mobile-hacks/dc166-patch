#include "test.h"

extern void _huge *hmalloc(unsigned long);
extern void _huge *hcalloc(unsigned long, unsigned long);
extern void _huge *hrealloc(void _huge *, unsigned long);
extern void hfree(void _huge *);


void main(void)
{
  unsigned char _huge *memory;
  unsigned char _huge *grown;
  unsigned long index;

  simulator_result = 0U;
  memory = (unsigned char _huge *)hcalloc(1UL, 70000UL);
  test_check(1U, memory != 0);
  if (memory != 0) {
    for (index = 0UL; index < 70000UL; index += 7001UL)
      test_check(2U, memory[index] == 0U);
    for (index = 0UL; index < 70000UL; index += 7001UL)
      memory[index] = (unsigned char)(index + 0x31UL);
  }

  grown = (unsigned char _huge *)hrealloc(memory, 80000UL);
  test_check(3U, grown != 0);
  if (grown != 0)
    for (index = 0UL; index < 70000UL; index += 7001UL)
      test_check(4U, grown[index] == (unsigned char)(index + 0x31UL));
  hfree(grown);

  memory = (unsigned char _huge *)hmalloc(70000UL);
  test_check(5U, memory != 0);
  hfree(memory);
  simulator_stop();
}
