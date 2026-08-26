#include "dc166_stdio.h"

static void initialize_wide_input(FILE *stream, const wchar_t *string)
{
  stream->_ptr = (char *)string;
  stream->_base = (char *)string;
  stream->_cnt = -(int)(wcslen(string) + 1U);
  stream->_bufsiz = 0;
  stream->_flag = _IORD | _IOSTRG;
  stream->_file = 0;
  stream->_wcnt = 0;
}

int swscanf(const wchar_t *string, const wchar_t *format, ...)
{
  FILE stream;
  va_list arguments;
  int result;
  initialize_wide_input(&stream, string);
  va_start(arguments, format);
  result = _dowscan(&stream, format, arguments);
  va_end(arguments);
  return result;
}

int vswscanf(const wchar_t *string, const wchar_t *format, va_list arguments)
{
  FILE stream;
  initialize_wide_input(&stream, string);
  return _dowscan(&stream, format, arguments);
}
