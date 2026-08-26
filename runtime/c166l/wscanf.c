#include "dc166_stdio.h"

int wscanf(const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = _dowscan(stdin, format, arguments);
  va_end(arguments);
  return (stdin->_flag & _IOERR) != 0U ? WEOF : result;
}

int vwscanf(const wchar_t *format, va_list arguments)
{
  int result = _dowscan(stdin, format, arguments);
  return (stdin->_flag & _IOERR) != 0U ? WEOF : result;
}
