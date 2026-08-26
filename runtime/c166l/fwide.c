#include "dc166_stdio.h"

int fwide(FILE *stream, int mode)
{
  if (mode < 0 && (stream->_flag & (_IOCHAR | _IOWCHAR)) == 0U)
    stream->_flag |= _IOCHAR;
  else if (mode > 0 && (stream->_flag & (_IOCHAR | _IOWCHAR)) == 0U)
    stream->_flag |= _IOWCHAR;

  if ((stream->_flag & _IOCHAR) != 0U)
    return -1;
  if ((stream->_flag & _IOWCHAR) != 0U)
    return 1;
  return 0;
}
