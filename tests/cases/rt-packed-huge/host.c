#include "test.h"

_packed struct packed_long {
  test_u8 prefix;
  test_i32 value;
};

typedef union {
  test_i32 value;
  test_u16 words[2];
} long_words;

static _packed struct packed_long _huge packed_source;
static _packed struct packed_long _huge packed_destination;

static void set_packed(_packed struct packed_long _huge *object,
                       test_u16 high, test_u16 low)
{
  test_u8 _huge *destination = (test_u8 _huge *)object;
  long_words source;
  test_u8 _near *source_bytes = (test_u8 _near *)&source;
  test_u16 index;

  source.words[0] = high;
  source.words[1] = low;
  object->prefix = 0xa5U;
  for (index = 0U; index < 4U; ++index)
    destination[index + 1U] = source_bytes[index];
}

static test_u16 packed_ok(_packed struct packed_long _huge *object,
                          test_u16 high, test_u16 low)
{
  test_u8 _huge *actual = (test_u8 _huge *)object;
  long_words expected;
  test_u8 _near *expected_bytes = (test_u8 _near *)&expected;
  test_u16 index;

  expected.words[0] = high;
  expected.words[1] = low;
  if (object->prefix != 0xa5U)
    return 0U;
  for (index = 0U; index < 4U; ++index)
    if (actual[index + 1U] != expected_bytes[index])
      return 0U;
  return 1U;
}

static void copy_packed(void)
{
  packed_destination.value = packed_source.value;
}

void main(void)
{
  static const test_u16 values[][2] = {
    {0x0000U, 0x0000U},
    {0x0000U, 0x0001U},
    {0x7fffU, 0xffffU},
    {0x8000U, 0x0000U},
    {0xffffU, 0xffffU},
    {0x1234U, 0x5678U},
    {0x89abU, 0xcdefU},
  };
  test_u16 index;

  simulator_result = 0U;
  for (index = 0U;
       index < (test_u16)(sizeof(values) / sizeof(values[0]));
       ++index) {
    set_packed(&packed_source, values[index][0], values[index][1]);
    packed_destination.prefix = 0xa5U;
    copy_packed();
    test_check((test_u16)(index + 1U),
               packed_ok(&packed_destination,
                         values[index][0], values[index][1]));
  }
  simulator_stop();
}
