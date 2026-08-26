#include "dc166_stdio.h"

int fgetpos(FILE *stream, fpos_t *position)
{
  long result;

  fflush(stream);
  result = _lseek((int)stream->_file, 0L, SEEK_CUR);
  if (result == -1L) {
    errno = ERR_POS;
    return EOF;
  }
  *position = (fpos_t)result;
  return 0;
}
