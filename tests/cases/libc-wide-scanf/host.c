#include "test.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>


static wchar_t pointer_text[40];
static int default_anchor;
static int _near near_anchor;

static void initialize_input(FILE *stream, const wchar_t *string)
{
  stream->_ptr = (char *)string;
  stream->_base = (char *)string;
  stream->_cnt = -(int)(wcslen(string) + 1U);
  stream->_bufsiz = 0;
  stream->_flag = _IORD | _IOSTRG;
  stream->_file = 0;
  stream->_wcnt = 0;
}

#ifndef DC166_SMALL_ORACLE
static int call_vswscanf(const wchar_t *input, const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = vswscanf(input, format, arguments);
  va_end(arguments);
  return result;
}

static int call_vfwscanf(FILE *stream, const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = vfwscanf(stream, format, arguments);
  va_end(arguments);
  return result;
}

static int call_vwscanf(const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = vwscanf(format, arguments);
  va_end(arguments);
  return result;
}
#endif

static void test_numbers(void)
{
  int decimal = 0;
  unsigned int hex = 0U;
  unsigned int octal = 0U;
  unsigned long large = 0UL;
  int count = -1;
  int result;

  result = swscanf(L" -123 0xff 077 4000000000!", L"%d %x %i %lu%n",
                   &decimal, &hex, &octal, &large, &count);
  test_check(1U, result == 4 && decimal == -123 && hex == 255U &&
                 octal == 63U && large == 4000000000UL && count == 25);
  if (simulator_result != 0U)
    return;

  decimal = 0;
  result = swscanf(L"skip:42", L"%*[^:]:%2d", &decimal);
  test_check(2U, result == 1 && decimal == 42);
}

static void test_characters_and_sets(void)
{
  char narrow[12];
  wchar_t wide[12];
  char character = 0;
  wchar_t wide_character = 0;
  int result;

  memset(narrow, 0, sizeof(narrow));
  result = swscanf(L"alpha B beta C", L"%s %c %ls %lc",
                   narrow, &character, wide, &wide_character);
  test_check(3U, result == 4 && strcmp(narrow, "alpha") == 0 &&
                 character == 'B' && wcscmp(wide, L"beta") == 0 &&
                 wide_character == 'C');
  if (simulator_result != 0U)
    return;

  result = swscanf(L"abc123 xyz", L"%[abc]%l[123] %[^q]",
                   narrow, wide, narrow + 4);
  test_check(4U, result == 3 && strcmp(narrow, "abc") == 0 &&
                 wcscmp(wide, L"123") == 0 &&
                 strcmp(narrow + 4, "xyz") == 0);
}

static void test_pointers_and_errors(void)
{
  void *default_value = 0;
  void _near *near_value = 0;
  void _near *near_source = (void _near *)&near_anchor;
  int value = 0;
  int result;

  swprintf(pointer_text, 40U, L"%p", (void *)&default_anchor);
  result = swscanf(pointer_text, L"%p", &default_value);
  test_check(5U, result == 1 && default_value == (void *)&default_anchor);
  if (simulator_result != 0U)
    return;

  swprintf(pointer_text, 40U, L"%hp", near_source);
  result = swscanf(pointer_text, L"%hp", &near_value);
  test_check(6U, result == 1 && near_value == near_source);
  if (simulator_result != 0U)
    return;

  errno = 0;
  result = swscanf(L"1.0", L"%f", &value);
  test_check(7U, result == 0 && errno == ERR_NOFLOAT);
  errno = 0;
  result = swscanf(L"x", L"%q", &value);
  test_check(8U, result == 0 && errno == ERR_FORMAT);
}

static void test_stream_wrappers(void)
{
  FILE stream;
  FILE saved_input = *stdin;
  int first = 0;
  int second = 0;

  initialize_input(&stream, L"11 22");
  test_check(9U, fwscanf(&stream, L"%d%d", &first, &second) == 2 &&
                 first == 11 && second == 22);

  initialize_input(stdin, L"33");
  test_check(10U, wscanf(L"%d", &first) == 1 && first == 33);
#ifndef DC166_SMALL_ORACLE
  first = 0;
  test_check(11U, call_vswscanf(L"44", L"%d", &first) == 1 &&
                  first == 44);
  first = 0;
  initialize_input(&stream, L"55");
  test_check(12U, call_vfwscanf(&stream, L"%d", &first) == 1 &&
                  first == 55);
  first = 0;
  initialize_input(stdin, L"66");
  test_check(13U, call_vwscanf(L"%d", &first) == 1 && first == 66);
#endif
  *stdin = saved_input;
}

void main(void)
{
  simulator_result = 0U;
  test_numbers();
  if (simulator_result != 0U)
    simulator_stop();
  test_characters_and_sets();
  if (simulator_result != 0U)
    simulator_stop();
  test_pointers_and_errors();
  if (simulator_result != 0U)
    simulator_stop();
  test_stream_wrappers();
  simulator_stop();
}
