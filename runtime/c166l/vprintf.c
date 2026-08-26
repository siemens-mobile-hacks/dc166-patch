#include "dc166_stdio.h"

int vprintf(const char *format, va_list arguments)
{
  int result = _doprint(stdout, format, arguments);
  return (stdout->_flag & _IOERR) != 0U ? EOF : result;
}
