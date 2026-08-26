#include "test.h"
#include <errno.h>
#include <stdio.h>


static void test_integer_bases(void)
{
  int decimal = 0;
  unsigned int unsigned_decimal = 0U;
  unsigned int octal = 0U;
  int automatic = 0;
  unsigned int hexadecimal = 0U;
  int result;

  result = sscanf(" \t-123 456 077 0x2a ff",
                  "%d %u %o %i %x", &decimal, &unsigned_decimal,
                  &octal, &automatic, &hexadecimal);
  test_check(1U, result == 5 && decimal == -123 &&
                 unsigned_decimal == 456U && octal == 63U &&
                 automatic == 42 && hexadecimal == 255U);

  decimal = 0;
  hexadecimal = 0U;
  automatic = -1;
  result = sscanf("12345Z", "%3d%2x%n", &decimal, &hexadecimal,
                  &automatic);
  test_check(2U, result == 2 && decimal == 123 &&
                 hexadecimal == 0x45U && automatic == 5);

  decimal = 0;
  automatic = 0;
  octal = 0U;
  result = sscanf("012 0x10 10", "%i %i %i", &decimal, &automatic,
                  &octal);
  test_check(3U, result == 3 && decimal == 10 && automatic == 16 &&
                 octal == 10U);
}

static void test_lengths_and_limits(void)
{
  int byte_signed = 0;
  unsigned int byte_unsigned = 0U;
  int normal_signed = 0;
  unsigned int normal_unsigned = 0U;
  long long_signed = 0L;
  unsigned long long_unsigned = 0UL;
  long count = -1L;
  int result;

  result = sscanf("999 -1 99999 99999 99999999999 4294967296 X",
                  "%hhd %hhu %d %u %ld %lu %ln",
                  &byte_signed, &byte_unsigned, &normal_signed,
                  &normal_unsigned, &long_signed, &long_unsigned, &count);
  test_check(40U, result == 6);
  test_check(41U, byte_signed == 127);
  test_check(42U, byte_unsigned == 65535U);
  test_check(43U, normal_signed == 32767);
  test_check(44U, normal_unsigned == 65535U);
  test_check(45U, long_signed == 2147483647L);
  test_check(46U, long_unsigned == 0xffffffffUL);
  test_check(47U, count == 42L);

  normal_signed = 0;
  long_signed = 0L;
  result = sscanf("-99999 -99999999999", "%d %ld",
                  &normal_signed, &long_signed);
  test_check(5U, result == 2 && normal_signed == -32768 &&
                 long_signed == (-2147483647L - 1L));
}

static void test_suppression_and_errors(void)
{
  int value = -1;
  int result;

  result = sscanf("11 22", "%*d %d", &value);
  test_check(6U, result == 1 && value == 22);

  value = -1;
  result = sscanf("x", "%d", &value);
  test_check(7U, result == 0 && value == -1);

  value = -1;
  result = sscanf("", "%d", &value);
  test_check(8U, result == EOF && value == -1);

  errno = 0;
  result = sscanf("12", "%q", &value);
  test_check(9U, result == 0 && errno == ERR_FORMAT);

  errno = 0;
  result = sscanf("1.5", "%f", &value);
  test_check(10U, result == 0 && value == -1 && errno == ERR_NOFLOAT);

  result = sscanf("abc", "abd");
  test_check(11U, result == 0);
}

static void test_extended_lengths(void)
{
  int intmax_value = 0;
  int size_value = 0;
  int difference_value = 0;
  int capital_value = 0;
  long long_long_value = 0L;
  int result;

  result = sscanf("70000 70000 70000 70000 70000",
                  "%jd %zd %td %Ld %lld", &intmax_value, &size_value,
                  &difference_value, &capital_value, &long_long_value);
  test_check(12U, result == 5 && intmax_value == 4464 &&
                  size_value == 32767 && difference_value == 32767 &&
                  capital_value == 32767 && long_long_value == 70000L);
}

void main(void)
{
  simulator_result = 0U;
  test_integer_bases();
  if (simulator_result != 0U)
    simulator_stop();
  test_lengths_and_limits();
  if (simulator_result != 0U)
    simulator_stop();
  test_suppression_and_errors();
  if (simulator_result != 0U)
    simulator_stop();
  test_extended_lengths();
  simulator_stop();
}
