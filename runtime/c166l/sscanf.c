#include "dc166_stdio.h"

int sscanf(const char *string, const char *format, ...)
{
  FILE stream;
  va_list arguments;
  int result;

  stream._ptr = (char *)string;
  stream._base = (char *)string;
  stream._cnt = -(int)(strlen(string) + 1U);
  stream._bufsiz = 0;
  stream._flag = _IORD | _IOSTRG;
  stream._file = 0;
  stream._wcnt = 0;
  va_start(arguments, format);
  result = _doscan(&stream, format, arguments);
  va_end(arguments);
  return result;
}
