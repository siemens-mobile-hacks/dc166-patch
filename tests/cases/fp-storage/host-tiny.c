#include "test.h"

_packed struct packed_double {
  test_u8 prefix;
  double value;
};

static const test_u16 vectors[][4] = {
  {0x0000U, 0x0000U, 0x0000U, 0x0000U},
  {0x8000U, 0x0000U, 0x0000U, 0x0000U},
  {0x0000U, 0x0000U, 0x0000U, 0x0001U},
  {0x000fU, 0xffffU, 0xffffU, 0xffffU},
  {0x0010U, 0x0000U, 0x0000U, 0x0000U},
  {0x3ff0U, 0x0000U, 0x0000U, 0x0000U},
  {0xc004U, 0x0000U, 0x0000U, 0x0000U},
  {0x7ff8U, 0x1234U, 0x5678U, 0x9abcU},
};

static volatile test_f64 source;
static volatile test_f64 destination;
static _packed struct packed_double packed_source;
static _packed struct packed_double packed_destination;

static void copy_aligned(volatile double *to, volatile double *from)
{
  *to = *from;
}

static void copy_packed(void)
{
  packed_destination.value = packed_source.value;
}

void main(void)
{
  test_u16 vector;
  test_u16 word;
  simulator_result = 0U;

  for (vector = 0U; vector < 8U; ++vector) {
    for (word = 0U; word < 4U; ++word)
      source.words[word] = vectors[vector][word];
    copy_aligned(&destination.value, &source.value);
    for (word = 0U; word < 4U; ++word)
      test_check((test_u16)(vector * 8U + word + 1U),
                 destination.words[word] == vectors[vector][word]);

    packed_source.prefix = 0xa5U;
    {
      test_u8 *to = (test_u8 *)&packed_source.value;
      test_u8 *from = (test_u8 *)&source.value;
      for (word = 0U; word < 8U; ++word)
        to[word] = from[word];
    }
    packed_destination.prefix = 0x5aU;
    copy_packed();
    test_check((test_u16)(vector * 8U + 5U),
               packed_destination.prefix == 0x5aU);
    {
      test_u8 *actual = (test_u8 *)&packed_destination.value;
      test_u8 *expected = (test_u8 *)&source.value;
      for (word = 0U; word < 8U; ++word)
        test_check((test_u16)(vector * 8U + word + 6U),
                   actual[word] == expected[word]);
    }
  }
  simulator_stop();
}
