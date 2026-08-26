#include "dc166_stdio.h"

int getc(FILE *stream)
{
  if (++stream->_cnt < 0)
    return (int)(unsigned char)*stream->_ptr++;
  return _filbuf(stream);
}
