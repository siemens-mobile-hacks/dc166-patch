#include "test.h"

typedef unsigned int size_t;

extern void *calloc(size_t, size_t);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);


static void fill(unsigned char *memory, size_t size, unsigned char seed)
{
  size_t index;

  for (index = 0U; index < size; ++index)
    memory[index] = (unsigned char)(seed + (unsigned char)index);
}

static test_u16 contents_equal(const unsigned char *memory, size_t size,
                               unsigned char seed)
{
  size_t index;

  for (index = 0U; index < size; ++index)
    if (memory[index] != (unsigned char)(seed + (unsigned char)index))
      return 0U;
  return 1U;
}

void main(void)
{
  unsigned char *first;
  unsigned char *second;
  unsigned char *zeroed;
  unsigned char *grown;
  unsigned char *replacement;
  size_t index;

  simulator_result = 0U;
  first = (unsigned char *)malloc(96U);
  second = (unsigned char *)malloc(160U);
  test_check(1U, first != 0 && second != 0 && first != second);
  fill(first, 96U, 0x10U);
  fill(second, 160U, 0x40U);
  test_check(2U, contents_equal(first, 96U, 0x10U));
  test_check(3U, contents_equal(second, 160U, 0x40U));

  zeroed = (unsigned char *)calloc(32U, 3U);
  test_check(4U, zeroed != 0);
  for (index = 0U; index < 96U; ++index)
    test_check(5U, zeroed[index] == 0U);

  grown = (unsigned char *)realloc(first, 192U);
  test_check(6U, grown != 0 && contents_equal(grown, 96U, 0x10U));
  fill(grown + 96U, 96U, 0x70U);
  grown = (unsigned char *)realloc(grown, 64U);
  test_check(7U, grown != 0 && contents_equal(grown, 64U, 0x10U));

  free(second);
  replacement = (unsigned char *)malloc(128U);
  test_check(8U, replacement != 0);
  fill(replacement, 128U, 0xa0U);
  test_check(9U, contents_equal(replacement, 128U, 0xa0U));
  test_check(10U, malloc(5000U) == 0);

  free(replacement);
  free(zeroed);
  free(grown);
  simulator_stop();
}
