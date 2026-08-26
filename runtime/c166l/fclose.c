#include "dc166_stdio.h"

dc166_free_function *_ptr_to_free;

int fclose(FILE *stream)
{
  int result;
  unsigned short flags = stream->_flag;

  if ((flags & (_IORD | _IOWR)) == 0U || (flags & _IOSTRG) != 0U) {
    errno = EINVAL;
    result = EOF;
  } else {
    if (stream->_cnt < -1)
      stream->_cnt = -1;
    result = fflush(stream);
    if ((flags & _IOMYBUF) != 0U && _ptr_to_free != 0)
      _ptr_to_free(stream->_base);
    if (_close((int)stream->_file) < 0)
      result = EOF;
    if ((flags & (_IOMYBUF | _IONBF)) != 0U)
      stream->_base = 0;
  }
  stream->_flag = 0U;
  stream->_cnt = 0;
  stream->_base = 0;
  stream->_ptr = 0;
  return result;
}
