#include "test.h"

typedef unsigned int size_t;

extern void _far *fmalloc(size_t);
extern void _far *fcalloc(size_t, size_t);
extern void _far *frealloc(void _far *, size_t);
extern void ffree(void _far *);
extern void _shuge *smalloc(size_t);
extern void _shuge *scalloc(size_t, size_t);
extern void _shuge *srealloc(void _shuge *, size_t);
extern void sfree(void _shuge *);
extern void _huge *hmalloc(unsigned long);
extern void _huge *hcalloc(unsigned long, unsigned long);
extern void _huge *hrealloc(void _huge *, unsigned long);
extern void hfree(void _huge *);


static void test_far(void)
{
  unsigned char _far *first = (unsigned char _far *)fmalloc(12000U);
  unsigned char _far *zeroed = (unsigned char _far *)fcalloc(64U, 2U);
  unsigned char _far *grown;
  size_t index;
  test_check(1U, first != 0 && zeroed != 0);
  if (first != 0)
    for (index = 0U; index < 12000U; index += 997U)
      first[index] = (unsigned char)(index + 3U);
  if (zeroed != 0)
    for (index = 0U; index < 128U; ++index)
      test_check(2U, zeroed[index] == 0U);
  grown = (unsigned char _far *)frealloc(first, 14000U);
  test_check(3U, grown != 0);
  if (grown != 0)
    for (index = 0U; index < 12000U; index += 997U)
      test_check(4U, grown[index] == (unsigned char)(index + 3U));
  ffree(grown);
  ffree(zeroed);
  test_check(5U, fmalloc(16379U) == 0);
}

static void test_shuge(void)
{
  unsigned char _shuge *memory = (unsigned char _shuge *)scalloc(150U, 200U);
  unsigned char _shuge *grown;
  size_t index;
  test_check(6U, memory != 0);
  if (memory != 0) {
    for (index = 0U; index < 30000U; index += 2003U)
      test_check(7U, memory[index] == 0U);
    for (index = 0U; index < 30000U; index += 2003U)
      memory[index] = (unsigned char)(index + 9U);
  }
  grown = (unsigned char _shuge *)srealloc(memory, 32000U);
  test_check(8U, grown != 0);
  if (grown != 0)
    for (index = 0U; index < 30000U; index += 2003U)
      test_check(9U, grown[index] == (unsigned char)(index + 9U));
  sfree(grown);
  memory = (unsigned char _shuge *)smalloc(32000U);
  test_check(15U, memory != 0);
  sfree(memory);
}

static void test_huge(void)
{
  unsigned char _huge *memory = (unsigned char _huge *)hcalloc(1UL, 50000UL);
  unsigned char _huge *grown;
  unsigned long index;
  test_check(10U, memory != 0);
  if (memory != 0) {
    for (index = 0UL; index < 50000UL; index += 5001UL)
      test_check(11U, memory[index] == 0U);
    for (index = 0UL; index < 50000UL; index += 5001UL)
      memory[index] = (unsigned char)(index + 0x31UL);
  }
  grown = (unsigned char _huge *)hrealloc(memory, 55000UL);
  test_check(12U, grown != 0);
  if (grown != 0)
    for (index = 0UL; index < 50000UL; index += 5001UL)
      test_check(13U, grown[index] == (unsigned char)(index + 0x31UL));
  hfree(grown);
  memory = (unsigned char _huge *)hmalloc(50000UL);
  test_check(14U, memory != 0);
  hfree(memory);
}

void main(void)
{
  simulator_result = 0U;
  test_far();
  if (simulator_result != 0U)
    simulator_stop();
  test_shuge();
  if (simulator_result != 0U)
    simulator_stop();
  test_huge();
  simulator_stop();
}
