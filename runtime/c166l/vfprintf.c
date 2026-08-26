#include "dc166_stdio.h"

int vfprintf(FILE *stream, const char *format, va_list arguments)
{
  int result = _doprint(stream, format, arguments);
  return (stream->_flag & _IOERR) != 0U ? EOF : result;
}
