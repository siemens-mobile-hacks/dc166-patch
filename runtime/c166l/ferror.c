#include "dc166_stdio.h"

int ferror(FILE *stream)
{
  return (stream->_flag & _IOERR) != 0U;
}
