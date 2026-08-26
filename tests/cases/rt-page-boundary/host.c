#include "test.h"

typedef union {
  test_u32 value;
  test_u16 words[2];
  test_u8 bytes[4];
} long_image;


static void write_aligned(test_u32 _huge *destination, test_u32 value)
{
  *destination = value;
}

static test_u32 read_aligned(test_u32 _huge *source)
{
  return *source;
}

static void write_packed(_noalign test_u32 _huge *destination,
                         test_u32 value)
{
  *destination = value;
}

static test_u32 read_packed(_noalign test_u32 _huge *source)
{
  return *source;
}

static void put_bytes(test_u8 _huge *destination, long_image *source)
{
  test_u16 index;

  for (index = 0U; index < 4U; ++index)
    destination[index] = source->bytes[index];
}

static void check_bytes(test_u16 id, test_u8 _huge *actual,
                        long_image *expected)
{
  test_u16 index;

  for (index = 0U; index < 4U; ++index)
    test_check((test_u16)(id + index),
               actual[index] == expected->bytes[index]);
}

void main(void)
{
  static const test_u16 values[][2] = {
    {0x0000U, 0x0000U},
    {0x0000U, 0x0001U},
    {0x7fffU, 0xffffU},
    {0x8000U, 0x0000U},
    {0x1234U, 0x5678U},
    {0x89abU, 0xcdefU},
    {0xffffU, 0xffffU},
  };
  test_u32 _huge *aligned = (test_u32 _huge *)0x20fffeUL;
  test_u8 _huge *aligned_bytes = (test_u8 _huge *)aligned;
  _noalign test_u32 _huge *packed =
      (_noalign test_u32 _huge *)0x21ffffUL;
  test_u8 _huge *packed_bytes = (test_u8 _huge *)packed;
  long_image expected;
  long_image actual;
  test_u16 index;
  test_u16 next;
  test_u16 id;

  simulator_result = 0U;
  id = 1U;
  for (index = 0U;
       index < (test_u16)(sizeof(values) / sizeof(values[0]));
       ++index) {
    next = (test_u16)((index + 1U) %
        (test_u16)(sizeof(values) / sizeof(values[0])));

    expected.words[0] = values[index][0];
    expected.words[1] = values[index][1];
    write_aligned(aligned, expected.value);
    check_bytes(id, aligned_bytes, &expected);
    id = (test_u16)(id + 4U);

    expected.words[0] = values[next][0];
    expected.words[1] = values[next][1];
    put_bytes(aligned_bytes, &expected);
    actual.value = read_aligned(aligned);
    test_check(id++, actual.words[0] == expected.words[0]);
    test_check(id++, actual.words[1] == expected.words[1]);

    expected.words[0] = values[index][0];
    expected.words[1] = values[index][1];
    write_packed(packed, expected.value);
    check_bytes(id, packed_bytes, &expected);
    id = (test_u16)(id + 4U);

    expected.words[0] = values[next][0];
    expected.words[1] = values[next][1];
    put_bytes(packed_bytes, &expected);
    actual.value = read_packed(packed);
    test_check(id++, actual.words[0] == expected.words[0]);
    test_check(id++, actual.words[1] == expected.words[1]);
  }
  simulator_stop();
}
