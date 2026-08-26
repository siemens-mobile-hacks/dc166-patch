#include "dc166_stdio.h"

int feof(FILE *stream)
{
  return (stream->_flag & _IOEOF) != 0U;
}
