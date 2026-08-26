#include "dc166_stdio.h"

int fseek(FILE *stream, long offset, int whence)
{
  fflush(stream);
  if (_lseek((int)stream->_file, offset, whence) == -1L)
    return EOF;
  stream->_flag &= (unsigned short)~_IOEOF;
  return 0;
}
