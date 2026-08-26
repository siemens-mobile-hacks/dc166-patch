#include "dc166_stdio.h"

FILE *_fopen(const char *name, const char *mode, FILE *stream)
{
  unsigned int open_flags = 0U;
  char option;
  int fd;

  if (stream == 0)
    return 0;

  do {
    option = *mode++;
    if (option == 'w') {
      open_flags |= 0x0301U;
      stream->_flag |= _IOWR;
    } else if (option == 'a') {
      open_flags |= 0x0109U;
      stream->_flag |= _IOWR;
    } else if (option == 'r') {
      stream->_flag |= _IORD;
    } else if (option == 'b') {
      open_flags |= 0x8000U;
    } else if (option == '8') {
      open_flags |= 0x4000U;
    } else if (option == '+') {
      open_flags |= 0x0002U;
      stream->_flag |= _IORD | _IOWR;
    } else if (open_flags == 0U) {
      return 0;
    }
  } while (*mode != 0);

  fd = _open(name, (int)open_flags);
  if (fd >= 0) {
    stream->_file = (char)fd;
    return stream;
  }
  stream->_flag = 0U;
  return 0;
}
