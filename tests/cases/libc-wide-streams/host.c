#include "test.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>


static char io_buffer[13];
static wchar_t wide_line[24];

static int wide_equal(const wchar_t *left, const wchar_t *right)
{
  while (*left != 0 && *left == *right) {
    ++left;
    ++right;
  }
  return *left == *right;
}

static int call_vfwprintf(FILE *stream, const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = vfwprintf(stream, format, arguments);
  va_end(arguments);
  return result;
}

static int call_vwprintf(const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = vwprintf(format, arguments);
  va_end(arguments);
  return result;
}

static void test_file_characters(void)
{
  const char *name = "dc166-wide-char.txt";
  FILE *stream;

  (void)remove(name);
  stream = fopen(name, "wb");
  test_check(1U, stream != 0);
  if (stream == 0)
    return;
  test_check(2U, setvbuf(stream, io_buffer, _IOLBF, sizeof(io_buffer)) == 0);
  test_check(3U, fputws(L"abc", stream) == 0 &&
                 stream->_ptr - stream->_base == 3);
  test_check(4U, fputwc('\n', stream) == '\n' &&
                 stream->_ptr == stream->_base);
  test_check(5U, fputws(L"tail", stream) == 0 && fclose(stream) == 0);

  stream = fopen(name, "rb");
  test_check(6U, stream != 0);
  if (stream == 0)
    return;
  test_check(7U, setvbuf(stream, io_buffer, _IOLBF, sizeof(io_buffer)) == 0);
  test_check(8U, fgetwc(stream) == 'a' && ungetwc('A', stream) == 'A' &&
                 fgetwc(stream) == 'A' && fgetwc(stream) == 'b');
  test_check(9U, fgetws(wide_line, 24, stream) == wide_line &&
                 wide_equal(wide_line, L"c"));
  test_check(10U, fgetws(wide_line, 24, stream) == wide_line &&
                  wide_equal(wide_line, L"tail") &&
                  fgetwc(stream) == WEOF && feof(stream));
  test_check(11U, fclose(stream) == 0 && remove(name) == 0);
}

static void test_formatted_streams(void)
{
  const char *name = "dc166-wide-format.txt";
  FILE *stream;

  (void)remove(name);
  stream = fopen(name, "wb");
  test_check(12U, stream != 0);
  if (stream == 0)
    return;
  test_check(13U, fwprintf(stream, L"%d %s ", 42, L"ok") == 6 &&
                  call_vfwprintf(stream, L"%X", 0xabcdU) == 4 &&
                  fclose(stream) == 0);
  stream = fopen(name, "rb");
  test_check(14U, stream != 0);
  if (stream != 0) {
    test_check(15U, fgetws(wide_line, 24, stream) == wide_line &&
                    wide_equal(wide_line, L"42 ok ABCD"));
    fclose(stream);
  }
  test_check(16U, remove(name) == 0);
}

static void test_standard_output_and_orientation(void)
{
  FILE saved_output = *stdout;
  wchar_t output[20];
  FILE stream;

  memset(stdout, 0, sizeof(*stdout));
  stdout->_ptr = (char *)output;
  stdout->_base = (char *)output;
  stdout->_cnt = 19;
  stdout->_flag = _IOWR | _IOSTRG;
  test_check(17U, wprintf(L"%s", L"A") == 1 &&
                  call_vwprintf(L"%d", 23) == 2 &&
                  fputwc(0, stdout) == 0 && wide_equal(output, L"A23"));
  *stdout = saved_output;

  memset(&stream, 0, sizeof(stream));
  stream._flag = _IOWR | _IONBF;
  stream._bufsiz = -1;
  stream._file = -1;
  errno = 0;
  test_check(18U, fputwc(0x100, &stream) == WEOF && errno == EILSEQ);
}

void main(void)
{
  simulator_result = 0U;
  test_file_characters();
  if (simulator_result != 0U)
    simulator_stop();
  test_formatted_streams();
  if (simulator_result != 0U)
    simulator_stop();
  test_standard_output_and_orientation();
  simulator_stop();
}
