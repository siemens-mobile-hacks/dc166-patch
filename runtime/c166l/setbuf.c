#include "dc166_stdio.h"

void setbuf(FILE *stream, char *buffer)
{
  if ((stream->_flag & _IOMYBUF) != 0U)
    free(stream->_base);
  stream->_flag &= (unsigned short)~(_IOMYBUF | _IONBF | _IOLBF);
  stream->_base = buffer;
  stream->_ptr = buffer;
  if (buffer == 0) {
    stream->_flag |= _IONBF;
    stream->_bufsiz = -1;
  } else {
    stream->_bufsiz = BUFSIZ;
  }
  stream->_cnt = 0;
}
