#include "dc166_stdio.h"

int vfwprintf(FILE *stream, const wchar_t *format, va_list arguments)
{
  int result = _dowprint(stream, format, arguments);
  return (stream->_flag & _IOERR) != 0U ? WEOF : result;
}
