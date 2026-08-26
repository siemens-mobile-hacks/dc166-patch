#include "dc166_stdio.h"

FILE *fopen(const char *name, const char *mode)
{
  FILE *stream = 0;
  int index;

  for (index = 0; index < _NFILE; ++index) {
    if (_iob[index]._flag == 0U) {
      stream = &_iob[index];
      break;
    }
  }
  if (stream == 0)
    return 0;

  stream->_flag = 0U;
  if (_fopen(name, mode, stream) == 0)
    return 0;
  stream->_base = (char *)malloc(BUFSIZ);
  stream->_ptr = stream->_base;
  stream->_cnt = 0;
  if (stream->_base != 0) {
    _ptr_to_free = free;
    stream->_flag |= _IOMYBUF;
    stream->_bufsiz = BUFSIZ;
  } else {
    stream->_flag |= _IONBF;
    stream->_bufsiz = -1;
  }
  return stream;
}
