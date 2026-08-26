#include "dc166_stdio.h"

int scanf(const char *format, ...)
{
  va_list arguments;
  int result;

  va_start(arguments, format);
  result = _doscan(stdin, format, arguments);
  va_end(arguments);
  return (stdin->_flag & _IOERR) != 0U ? EOF : result;
}
