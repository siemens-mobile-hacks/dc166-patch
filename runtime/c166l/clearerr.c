#include "dc166_stdio.h"

void clearerr(FILE *stream)
{
  stream->_flag &= (unsigned short)~(_IOEOF | _IOERR);
}
