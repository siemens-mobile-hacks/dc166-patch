#include "dc166_stdio.h"

int fflush(FILE *stream)
{
  int result = 0;
  int *count;
  long seek_offset = 0L;
  char *base;
  int bytes;
  int index;

  if (stream == 0) {
    for (index = 0; index < _NFILE; ++index)
      result |= fflush(&_iob[index]);
    return result;
  }

  count = (stream->_flag & _IOWCHAR) == 0U ?
      &stream->_cnt : &stream->_wcnt;
  if ((stream->_flag & _IONBF) == 0U) {
    base = stream->_base;
    if (*count < -1) {
      seek_offset = (long)*count + 1L;
    } else if (*count >= 0) {
      bytes = (int)(stream->_ptr - base);
      if (bytes > 0 &&
          (int)_write((int)stream->_file, base,
                      (unsigned int)bytes) != bytes) {
        stream->_flag |= _IOERR;
        result = EOF;
      }
    }
    stream->_ptr = base;
  } else if (stream->_bufsiz != -1) {
    seek_offset = -1L;
  }

  if (seek_offset != 0L &&
      _lseek((int)stream->_file, seek_offset, SEEK_CUR) == -1L) {
    stream->_flag |= _IOERR;
    result = EOF;
  }
  *count = 0;
  return result;
}
