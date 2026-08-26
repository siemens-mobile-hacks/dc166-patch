#include "dc166_stdio.h"

int swprintf(wchar_t *string, size_t count, const wchar_t *format, ...)
{
  FILE stream;
  va_list arguments;
  int result;

  stream._ptr = (char *)string;
  stream._base = (char *)string;
  stream._cnt = (int)count - 1;
  stream._bufsiz = 0;
  stream._flag = _IOWR | _IOSTRG;
  stream._file = 0;
  stream._wcnt = 0;
  va_start(arguments, format);
  result = _dowprint(&stream, format, arguments);
  va_end(arguments);
  if ((size_t)result < count) {
    if (count != 0U) {
      stream._cnt = 1;
      fputwc(0, &stream);
    }
    return result;
  }
  if (count != 0U)
    string[count - 1U] = 0;
  return -1;
}
