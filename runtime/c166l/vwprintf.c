#include "dc166_stdio.h"

int vwprintf(const wchar_t *format, va_list arguments)
{
  int result = _dowprint(stdout, format, arguments);
  return (stdout->_flag & _IOERR) != 0U ? WEOF : result;
}
