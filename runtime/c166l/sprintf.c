#include "dc166_stdio.h"

int sprintf(char *string, const char *format, ...)
{
  FILE stream;
  va_list arguments;
  int result;

  stream._ptr = string;
  stream._base = string;
  stream._cnt = 0x7fff;
  stream._bufsiz = 0;
  stream._flag = _IOWR | _IOSTRG;
  stream._file = 0;
  stream._wcnt = 0;
  va_start(arguments, format);
  result = _doprint(&stream, format, arguments);
  va_end(arguments);
  fputc(0, &stream);
  return result;
}
