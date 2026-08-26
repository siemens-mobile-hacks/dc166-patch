#include "dc166_stdio.h"

int fprintf(FILE *stream, const char *format, ...)
{
  va_list arguments;
  int result;

  va_start(arguments, format);
  result = _doprint(stream, format, arguments);
  va_end(arguments);
  return (stream->_flag & _IOERR) != 0U ? EOF : result;
}
