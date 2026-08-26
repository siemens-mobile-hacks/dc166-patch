#include "dc166_stdio.h"

int vsprintf(char *string, const char *format, va_list arguments)
{
  FILE stream;
  int result;

  stream._ptr = string;
  stream._base = string;
  stream._cnt = 0x7fff;
  stream._bufsiz = 0;
  stream._flag = _IOWR | _IOSTRG;
  stream._file = 0;
  stream._wcnt = 0;
  result = _doprint(&stream, format, arguments);
  fputc(0, &stream);
  return result;
}
