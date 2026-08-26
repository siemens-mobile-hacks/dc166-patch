#include "test.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>


static wchar_t text[128];
static wchar_t storage[32];
static int default_anchor;
static int _near near_anchor;

static int call_vswprintf(wchar_t *destination, size_t count,
                          const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = vswprintf(destination, count, format, arguments);
  va_end(arguments);
  return result;
}

static int wide_hex(wchar_t character)
{
  return (character >= '0' && character <= '9') ||
         (character >= 'A' && character <= 'F');
}

static int wide_hex_range(const wchar_t *string, unsigned int begin,
                          unsigned int end)
{
  unsigned int index;
  for (index = begin; index < end; ++index) {
    if (!wide_hex(string[index]))
      return 0;
  }
  return 1;
}

static void test_formatted_output(void)
{
  int count16 = -1;
  long count32 = -1L;
  int result;

  result = swprintf(text, 128U, L"plain%%:%c:%lc:%s", 'Q', 'R', L"ok");
  test_check(1U, result == 13 && wcscmp(text, L"plain%:Q:R:ok") == 0);
  if (simulator_result != 0U)
    return;

  result = swprintf(text, 128U, L"%d|%u|%o|%x|%X", (int)-32768L,
                    65535U, 83U, 0xabcdU, 0xabcdU);
  test_check(2U, result == 26 &&
                 wcscmp(text, L"-32768|65535|123|abcd|ABCD") == 0);
  if (simulator_result != 0U)
    return;

  result = swprintf(text, 128U, L"%ld|%lu", -12345678L, 4000000000UL);
  test_check(3U, result == 20 &&
                 wcscmp(text, L"-12345678|4000000000") == 0);
  if (simulator_result != 0U)
    return;

  result = call_vswprintf(text, 128U, L"%f:%d", 1.25, 77);
  test_check(4U, result == 14 && wcscmp(text, L"<no floats>:77") == 0);
  if (simulator_result != 0U)
    return;

  result = swprintf(text, 128U, L"abc%nXYZ%ln!", &count16, &count32);
  test_check(5U, result == 7 && wcscmp(text, L"abcXYZ!") == 0 &&
                 count16 == 3 && count32 == 6L);
  if (simulator_result != 0U)
    return;

  result = swprintf(text, 5U, L"abcdef");
  test_check(6U, result == -1 && wcscmp(text, L"abcd") == 0);
  if (simulator_result != 0U)
    return;

  errno = 0;
  result = swprintf(text, 128U, L"abc%qTAIL", 1);
  test_check(7U, result == 3 && wcscmp(text, L"abc") == 0 &&
                 errno == ERR_FORMAT);
}

static void test_pointers(void)
{
  void _near *near_source = (void _near *)&near_anchor;
  int result;

  result = swprintf(text, 128U, L"%p", (void *)&default_anchor);
#if _MODEL == 'm' || _MODEL == 'l'
  test_check(8U, result == 14 && wcsncmp(text, L"<far>", 5U) == 0 &&
                 text[9] == ':' && wide_hex_range(text, 5U, 9U) &&
                 wide_hex_range(text, 10U, 14U));
#elif _MODEL == 'h'
  test_check(8U, result == 13 && wcsncmp(text, L"<huge>", 6U) == 0 &&
                 text[8] == ':' && wide_hex_range(text, 6U, 8U) &&
                 wide_hex_range(text, 9U, 13U));
#else
  test_check(8U, result == 10 && wcsncmp(text, L"<near>", 6U) == 0 &&
                 wide_hex_range(text, 6U, 10U));
#endif
  if (simulator_result != 0U)
    return;

  result = swprintf(text, 128U, L"%hp:%d", near_source, 77);
  test_check(9U, result == 13 && wcsncmp(text, L"<near>", 6U) == 0 &&
                 wide_hex_range(text, 6U, 10U) &&
                 wcscmp(text + 10, L":77") == 0);
}

static void test_memory_streams(void)
{
  FILE stream;
  wchar_t line[8];

  memset(&stream, 0, sizeof(stream));
  stream._ptr = (char *)storage;
  stream._base = (char *)storage;
  stream._cnt = 4;
  stream._flag = _IOWR | _IOSTRG;
  test_check(10U, fputwc('A', &stream) == 'A' &&
                  putwc('B', &stream) == 'B' &&
                  fputws(L"CD", &stream) == 0 &&
                  fputwc('E', &stream) == WEOF &&
                  storage[0] == 'A' && storage[3] == 'D');

  wcscpy(storage, L"xy\nz");
  memset(&stream, 0, sizeof(stream));
  stream._ptr = (char *)storage;
  stream._base = (char *)storage;
  stream._cnt = -(int)(wcslen(storage) + 1U);
  stream._flag = _IORD | _IOSTRG;
  test_check(11U, fgetwc(&stream) == 'x' && getwc(&stream) == 'y' &&
                  ungetwc('Y', &stream) == 'Y' && fgetwc(&stream) == 'y');
  test_check(120U, fgetws(line, 8, &stream) == line);
  test_check(121U, wcscmp(line, L"") == 0);
  test_check(122U, fgetwc(&stream) == 'z');
  test_check(123U, fgetwc(&stream) == WEOF);

  memset(&stream, 0, sizeof(stream));
  stream._flag = _IORD;
  test_check(13U, fwide(&stream, 0) == 0 && fwide(&stream, 1) == 1 &&
                  fwide(&stream, -1) == 1);
  memset(&stream, 0, sizeof(stream));
  stream._flag = _IOWR;
  test_check(14U, fwide(&stream, -1) == -1 && fwide(&stream, 1) == -1);

  {
    FILE saved_input = *stdin;
    FILE saved_output = *stdout;
    wcscpy(storage, L"I");
    memset(stdin, 0, sizeof(*stdin));
    stdin->_ptr = (char *)storage;
    stdin->_base = (char *)storage;
    stdin->_cnt = -2;
    stdin->_flag = _IORD | _IOSTRG;
    memset(stdout, 0, sizeof(*stdout));
    stdout->_ptr = (char *)(storage + 4);
    stdout->_base = (char *)(storage + 4);
    stdout->_cnt = 1;
    stdout->_flag = _IOWR | _IOSTRG;
    test_check(15U, getwchar() == 'I' && putwchar('O') == 'O' &&
                    storage[4] == 'O');
    *stdin = saved_input;
    *stdout = saved_output;
  }
}

void main(void)
{
  simulator_result = 0U;
  test_formatted_output();
  if (simulator_result != 0U)
    simulator_stop();
  test_pointers();
  if (simulator_result != 0U)
    simulator_stop();
  test_memory_streams();
  simulator_stop();
}
