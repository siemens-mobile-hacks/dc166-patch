#include "dc166_stdio.h"

long ftell(FILE *stream)
{
  long result;

  fflush(stream);
  result = _lseek((int)stream->_file, 0L, SEEK_CUR);
  if (result == -1L)
    errno = ERR_POS;
  return result;
}
