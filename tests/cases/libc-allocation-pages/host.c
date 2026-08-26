#include "test.h"

typedef unsigned int size_t;

extern void *calloc(size_t, size_t);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);


static unsigned long linear_address(void *memory)
{
  return (unsigned long)(unsigned char _huge *)memory;
}

static void put_samples(unsigned char *memory, size_t size,
                        unsigned char value)
{
  size_t index;

  for (index = 0U; index < 8U; ++index)
    memory[(size_t)(((unsigned long)(size - 1U) * index) / 7UL)] =
        (unsigned char)(value + (unsigned char)index);
}

static test_u16 samples_equal(unsigned char *memory, size_t size,
                              unsigned char value)
{
  size_t index;

  for (index = 0U; index < 8U; ++index)
    if (memory[(size_t)(((unsigned long)(size - 1U) * index) / 7UL)] !=
        (unsigned char)(value + (unsigned char)index))
      return 0U;
  return 1U;
}

void main(void)
{
  unsigned char *blocks[6];
  unsigned char *large;
  unsigned char *grown;
  test_u16 index;
  unsigned long address;

  simulator_result = 0U;
  for (index = 0U; index < 6U; ++index) {
    blocks[index] = (unsigned char *)malloc(12000U);
    test_check(1U, blocks[index] != 0);
    if (blocks[index] != 0) {
      address = linear_address(blocks[index]);
#if _MODEL != 'h'
      test_check(2U, (address & 0x3fffUL) + 12000UL <= 0x4000UL);
#endif
      put_samples(blocks[index], 12000U,
                  (unsigned char)(0x20U + index * 0x10U));
    }
  }
  for (index = 0U; index < 6U; ++index)
    if (blocks[index] != 0)
      test_check(3U, samples_equal(blocks[index], 12000U,
                     (unsigned char)(0x20U + index * 0x10U)));

#if _MODEL == 'h'
  large = (unsigned char *)malloc(20000U);
  test_check(4U, large != 0);
  if (large != 0) {
    put_samples(large, 20000U, 0x90U);
    test_check(5U, samples_equal(large, 20000U, 0x90U));
    free(large);
  }
#else
  test_check(4U, malloc(16379U) == 0);
#endif
  test_check(5U, calloc(256U, 256U) == 0);

  for (index = 0U; index < 6U; index += 2U)
    free(blocks[index]);
  for (index = 1U; index < 6U; index += 2U)
    free(blocks[index]);

  large = (unsigned char *)malloc(16000U);
  test_check(6U, large != 0);
  if (large != 0) {
    address = linear_address(large);
#if _MODEL != 'h'
    test_check(7U, (address & 0x3fffUL) + 16000UL <= 0x4000UL);
#endif
    put_samples(large, 16000U, 0xa0U);
    grown = (unsigned char *)realloc(large, 16200U);
    test_check(8U, grown != 0 && samples_equal(grown, 16000U, 0xa0U));
    free(grown);
  }
  simulator_stop();
}
