#include <stdarg.h>
#include "test.h"

_packed struct packed3 {
  test_u8 first;
  test_u16 second;
};

test_u16 fixed_packed_call(test_u16 fixed, _packed struct packed3 value,
                           test_u16 tail)
{
  return (test_u16)(fixed * 3U + (test_u16)value.first * 19U +
                    value.second * 23U + tail * 29U);
}

test_u16 variadic_packed_call(test_u16 fixed, ...)
{
  va_list arguments;
  test_u8 *bytes;
  test_u8 first;
  test_u16 second;
  test_u16 tail;

  va_start(arguments, fixed);
  bytes = (test_u8 *)arguments;
  first = bytes[0];
  second = (test_u16)(bytes[1] | (test_u16)bytes[2] << 8);
  tail = (test_u16)(bytes[4] | (test_u16)bytes[5] << 8);
  va_end(arguments);
  return (test_u16)(fixed * 3U + (test_u16)first * 19U +
                    second * 23U + tail * 29U);
}
