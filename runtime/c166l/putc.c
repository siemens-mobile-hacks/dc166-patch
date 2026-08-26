#include "dc166_stdio.h"

int putc(int character, FILE *stream)
{
  unsigned char byte = (unsigned char)character;

  if (--stream->_cnt < 0)
    return _flsbuf((int)byte, stream);
  *stream->_ptr++ = (char)byte;
  return (int)byte;
}
