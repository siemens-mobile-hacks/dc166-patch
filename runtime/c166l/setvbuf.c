#include "dc166_stdio.h"

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
  int result = 0;
  int buffering = mode & _IOBUF;

  if (stream->_flag == 0U ||
      (buffering != _IOFBF && buffering != _IOLBF &&
       buffering != _IONBF) ||
      (buffering == _IONBF && buffer != 0))
    return 1;

  if ((stream->_flag & _IOMYBUF) != 0U && _ptr_to_free != 0)
    _ptr_to_free(stream->_base);
  stream->_flag &= (unsigned short)~(_IOMYBUF | _IONBF | _IOLBF);

  if (buffering != _IONBF) {
    if (buffer == 0) {
      buffer = (char *)malloc(size);
      if (buffer != 0) {
        _ptr_to_free = free;
        stream->_flag |= _IOMYBUF;
      } else {
        buffering = _IONBF;
        result = 1;
      }
    }
    if (buffering != _IONBF && size == 0U)
      buffering = _IONBF;
  } else {
    buffering = _IONBF;
  }

  if (buffering == _IONBF) {
    stream->_flag |= _IONBF;
    stream->_base = 0;
    stream->_bufsiz = -1;
  } else {
    stream->_flag |= (unsigned short)buffering;
    stream->_base = buffer;
    stream->_bufsiz = (int)size;
  }
  stream->_ptr = stream->_base;
  stream->_cnt = 0;
  return result;
}
