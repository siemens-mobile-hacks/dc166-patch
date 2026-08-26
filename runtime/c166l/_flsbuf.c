#include "dc166_stdio.h"

int _flsbuf(int character, FILE *stream)
{
  unsigned char byte = (unsigned char)character;
  int bytes;
  int capacity;

  if ((stream->_flag & _IOWCHAR) != 0U)
    return EOF;
  stream->_flag |= _IOCHAR;
  if ((stream->_flag & _IOWR) == 0U)
    goto write_error;

  if ((stream->_flag & _IONBF) != 0U) {
    stream->_cnt = 0;
    if ((int)_write((int)stream->_file, (char *)&byte, 1U) != 1)
      goto write_error;
    return (int)byte;
  }

  if (stream->_cnt < -2) {
    ++stream->_cnt;
    fflush(stream);
  }
  if (stream->_base == 0)
    goto write_error;

  if ((stream->_flag & _IOLBF) != 0U) {
    if (stream->_ptr == 0)
      stream->_ptr = stream->_base;
    stream->_cnt = 0;
    if ((unsigned int)(stream->_ptr - stream->_base) <
        (unsigned int)stream->_bufsiz)
      *stream->_ptr++ = (char)byte;
    if ((stream->_ptr - stream->_base) != stream->_bufsiz &&
        byte != (unsigned char)'\n')
      return (int)byte;
  }

  bytes = stream->_ptr == 0 ? 0 :
      (int)(stream->_ptr - stream->_base);
  if (bytes > 0 &&
      (int)_write((int)stream->_file, stream->_base,
                  (unsigned int)bytes) != bytes) {
    stream->_flag |= _IOERR;
    stream->_cnt = stream->_bufsiz == 0 ? BUFSIZ : stream->_bufsiz;
    stream->_ptr = stream->_base;
    return EOF;
  }

  stream->_ptr = stream->_base;
  if ((stream->_flag & _IOLBF) == 0U) {
    capacity = stream->_bufsiz == 0 ? BUFSIZ : stream->_bufsiz;
    stream->_cnt = capacity - 1;
    *stream->_ptr++ = (char)byte;
  }
  return (int)byte;

write_error:
  stream->_flag |= _IOERR;
  return EOF;
}
