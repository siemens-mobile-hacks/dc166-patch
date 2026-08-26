#include "dc166_stdio.h"

int fputc(int character, FILE *stream)
{
  unsigned char byte = (unsigned char)character;

  if ((stream->_flag & (_IOWR | _IOSTRG)) == (_IOWR | _IOSTRG)) {
    if (--stream->_cnt < 0)
      return EOF;
    *stream->_ptr++ = (char)byte;
    return (int)byte;
  }
  if (--stream->_cnt < 0)
    return _flsbuf((int)byte, stream);
  *stream->_ptr++ = (char)byte;
  return (int)byte;
}
