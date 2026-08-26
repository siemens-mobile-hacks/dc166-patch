#include "test.h"

typedef unsigned int size_t;

extern void _near *nmalloc(size_t);
extern void _near *ncalloc(size_t, size_t);
extern void _near *nrealloc(void _near *, size_t);
extern void nfree(void _near *);


void main(void)
{
  unsigned char _near *first;
  unsigned char _near *second;
  unsigned char _near *grown;
  size_t index;

  simulator_result = 0U;
  first = (unsigned char _near *)nmalloc(96U);
  second = (unsigned char _near *)ncalloc(48U, 2U);
  test_check(1U, first != 0 && second != 0 && first != second);
  for (index = 0U; index < 96U; ++index) {
    first[index] = (unsigned char)(index + 0x20U);
    test_check(2U, second[index] == 0U);
  }
  grown = (unsigned char _near *)nrealloc(first, 192U);
  test_check(3U, grown != 0);
  if (grown != 0)
    for (index = 0U; index < 96U; ++index)
      test_check(4U, grown[index] == (unsigned char)(index + 0x20U));
  first = (unsigned char _near *)nrealloc(grown, 64U);
  test_check(5U, first != 0);
  if (first != 0)
    for (index = 0U; index < 64U; ++index)
      test_check(5U, first[index] == (unsigned char)(index + 0x20U));
  grown = first;
  nfree(grown);
  nfree(second);
  first = (unsigned char _near *)nmalloc(256U);
  test_check(6U, first != 0);
  nfree(first);
  simulator_stop();
}
