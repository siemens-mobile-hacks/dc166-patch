#include "test.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


static char text[128];
static int default_anchor;
static int _near near_anchor;

static int call_vsprintf(char *destination, const char *format, ...)
{
  va_list arguments;
  int result;

  va_start(arguments, format);
  result = vsprintf(destination, format, arguments);
  va_end(arguments);
  return result;
}

static int is_upper_hex(char character)
{
  return (character >= '0' && character <= '9') ||
         (character >= 'A' && character <= 'F');
}

static int is_hex_range(const char *string, unsigned int begin,
                        unsigned int end)
{
  unsigned int index;

  for (index = begin; index < end; ++index) {
    if (!is_upper_hex(string[index]))
      return 0;
  }
  return 1;
}

static void test_basic_and_integers(void)
{
  int result;

  result = sprintf(text, "plain%%:%c:%s", 'Q', "ok");
  test_check(1U, result == 11 && strcmp(text, "plain%:Q:ok") == 0);
  if (simulator_result != 0U)
    return;

  result = sprintf(text, "%d|%i|%u|%o|%x|%X",
                   (int)-32768L, 123, 65535U, 83U, 0xabcdU, 0xabcdU);
  test_check(2U, result == 30 &&
                 strcmp(text, "-32768|123|65535|123|abcd|ABCD") == 0);
  if (simulator_result != 0U)
    return;

  result = sprintf(text, "%hd|%hu", -123, 65000U);
  test_check(3U, result == 10 && strcmp(text, "-123|65000") == 0);
  if (simulator_result != 0U)
    return;

  result = sprintf(text, "%ld|%lu|%lo|%lx|%lX", -12345678L,
                   4000000000UL, 0x89abcdefUL, 0x89abcdefUL,
                   0x89abcdefUL);
  test_check(4U, result == 50 &&
                 strcmp(text,
                   "-12345678|4000000000|21152746757|89abcdef|89ABCDEF") == 0);
}

static void test_varargs_float_count_and_error(void)
{
  int count16 = -1;
  long count32 = -1L;
  int result;

  result = call_vsprintf(text, "V:%s:%d", "ok", -7);
  test_check(5U, result == 7 && strcmp(text, "V:ok:-7") == 0);
  if (simulator_result != 0U)
    return;

  result = sprintf(text, "%f:%d", 1.25, 77);
  test_check(6U, result == 14 && strcmp(text, "<no floats>:77") == 0);
  if (simulator_result != 0U)
    return;

  result = sprintf(text, "abc%nXYZ%ln!", &count16, &count32);
  test_check(7U, result == 7 && strcmp(text, "abcXYZ!") == 0 &&
                 count16 == 3 && count32 == 6L);
  if (simulator_result != 0U)
    return;

  errno = 0;
  result = sprintf(text, "abc%qTAIL", 1);
  test_check(8U, result == 3 && strcmp(text, "abc") == 0 &&
                 errno == ERR_FORMAT);
}

static void test_pointers(void)
{
  void _near *near_source = (void _near *)&near_anchor;
  int result;

  result = sprintf(text, "%p", (void *)&default_anchor);
#if _MODEL == 'm' || _MODEL == 'l'
  test_check(9U, result == 14 && strncmp(text, "<far>", 5U) == 0 &&
                 text[9] == ':' && is_hex_range(text, 5U, 9U) &&
                 is_hex_range(text, 10U, 14U));
#elif _MODEL == 'h'
  test_check(9U, result == 13 && strncmp(text, "<huge>", 6U) == 0 &&
                 text[8] == ':' && is_hex_range(text, 6U, 8U) &&
                 is_hex_range(text, 9U, 13U));
#else
  test_check(9U, result == 10 && strncmp(text, "<near>", 6U) == 0 &&
                 is_hex_range(text, 6U, 10U));
#endif
  if (simulator_result != 0U)
    return;

  result = sprintf(text, "%hp:%d", near_source, 77);
  test_check(10U, result == 13 && strncmp(text, "<near>", 6U) == 0 &&
                  is_hex_range(text, 6U, 10U) &&
                  strcmp(text + 10, ":77") == 0);
  if (simulator_result != 0U)
    return;

#if _MODEL != 't'
  result = sprintf(text, "%lp", (void _far *)&default_anchor);
  test_check(11U, result == 14 && strncmp(text, "<far>", 5U) == 0 &&
                  text[9] == ':' && is_hex_range(text, 5U, 9U) &&
                  is_hex_range(text, 10U, 14U));
  if (simulator_result != 0U)
    return;

  result = sprintf(text, "%Lp", (void _huge *)&default_anchor);
  test_check(12U, result == 13 && strncmp(text, "<huge>", 6U) == 0 &&
                  text[8] == ':' && is_hex_range(text, 6U, 8U) &&
                  is_hex_range(text, 9U, 13U));
#endif
}

void main(void)
{
  simulator_result = 0U;
  test_basic_and_integers();
  if (simulator_result != 0U)
    simulator_stop();
  test_varargs_float_count_and_error();
  if (simulator_result != 0U)
    simulator_stop();
  test_pointers();
  simulator_stop();
}
