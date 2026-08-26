#include "dc166_stdio.h"

int fsetpos(FILE *stream, const fpos_t *position)
{
  fflush(stream);
  if (_lseek((int)stream->_file, (long)*position, SEEK_SET) == -1L) {
    errno = ERR_POS;
    return EOF;
  }
  stream->_flag &= (unsigned short)~_IOEOF;
  return 0;
}
