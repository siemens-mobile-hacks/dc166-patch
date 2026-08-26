#include "dc166_stdio.h"

int printf(const char *format, ...)
{
  va_list arguments;
  int result;

  va_start(arguments, format);
  result = _doprint(stdout, format, arguments);
  va_end(arguments);
  return (stdout->_flag & _IOERR) != 0U ? EOF : result;
}
