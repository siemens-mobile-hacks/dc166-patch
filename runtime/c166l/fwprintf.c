#include "dc166_stdio.h"

int fwprintf(FILE *stream, const wchar_t *format, ...)
{
  va_list arguments;
  int result;
  va_start(arguments, format);
  result = _dowprint(stream, format, arguments);
  va_end(arguments);
  return (stream->_flag & _IOERR) != 0U ? WEOF : result;
}
