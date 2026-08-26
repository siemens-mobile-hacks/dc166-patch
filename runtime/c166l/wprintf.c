#include "dc166_stdio.h"

int wprintf(const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = _dowprint(stdout, format, arguments);
  va_end(arguments);
  return (stdout->_flag & _IOERR) != 0U ? WEOF : result;
}
