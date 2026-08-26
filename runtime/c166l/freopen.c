#include "dc166_stdio.h"

FILE *freopen(const char *name, const char *mode, FILE *stream)
{
  unsigned short old_flags =
      stream->_flag & (_IOMYBUF | _IONBF | _IOLBF);
  char *old_base = stream->_base;

  stream->_flag &= (unsigned short)~_IOMYBUF;
  fclose(stream);
  if (_fopen(name, mode, stream) == 0) {
    if ((old_flags & _IOMYBUF) != 0U && _ptr_to_free != 0)
      _ptr_to_free(old_base);
    return 0;
  }

  if ((old_flags & _IOMYBUF) == 0U || old_base == 0) {
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
  } else {
    stream->_flag |= old_flags;
    stream->_base = old_base;
    stream->_ptr = old_base;
  }
  stream->_flag &= (unsigned short)~(_IOCHAR | _IOWCHAR);
  return stream;
}
