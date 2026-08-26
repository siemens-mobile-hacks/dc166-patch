#include "dc166_stdio.h"

int ungetc(int character, FILE *stream)
{
  unsigned char byte = (unsigned char)character;

  if (character == EOF || (stream->_flag & _IORD) == 0U)
    return EOF;
  stream->_flag &= (unsigned short)~_IOEOF;

  if ((stream->_flag & _IOSTRG) != 0U) {
    if (stream->_ptr <= stream->_base)
      return EOF;
    --stream->_ptr;
    --stream->_cnt;
    return (int)byte;
  }

  if ((stream->_flag & _IONBF) == 0U) {
    if (stream->_cnt >= 0) {
      fflush(stream);
      stream->_ptr = stream->_base + stream->_bufsiz;
      stream->_cnt = -1;
    }
    --stream->_cnt;
    *--stream->_ptr = (char)byte;
    return (int)byte;
  }

  if (stream->_bufsiz != -1)
    return EOF;
  stream->_bufsiz = (int)byte;
  return (int)byte;
}
