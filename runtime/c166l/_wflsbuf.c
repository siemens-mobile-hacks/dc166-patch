#include "dc166_stdio.h"

wint_t _wflsbuf(wchar_t character, FILE *stream)
{
  unsigned char byte = (unsigned char)character;
  int bytes;
  int capacity;

  if ((stream->_flag & _IOCHAR) != 0U)
    return WEOF;
  stream->_flag |= _IOWCHAR;
  if ((stream->_flag & _IOWR) == 0U)
    goto write_error;

  if ((stream->_flag & _IONBF) != 0U) {
    stream->_wcnt = 0;
    if ((int)_write((int)stream->_file, (char *)&byte, 1U) != 1)
      goto write_error;
    return (wint_t)byte;
  }

  if (stream->_wcnt < -2) {
    ++stream->_wcnt;
    fflush(stream);
  }
  if (stream->_base == 0)
    goto write_error;

  if ((stream->_flag & _IOLBF) != 0U) {
    if (stream->_ptr == 0)
      stream->_ptr = stream->_base;
    stream->_wcnt = 0;
    if ((unsigned int)(stream->_ptr - stream->_base) <
        (unsigned int)stream->_bufsiz)
      *stream->_ptr++ = (char)byte;
    if ((stream->_ptr - stream->_base) != stream->_bufsiz &&
        byte != (unsigned char)'\n')
      return (wint_t)byte;
  }

  bytes = stream->_ptr == 0 ? 0 : (int)(stream->_ptr - stream->_base);
  if (bytes > 0 &&
      (int)_write((int)stream->_file, stream->_base,
                  (unsigned int)bytes) != bytes) {
    stream->_flag |= _IOERR;
    stream->_wcnt = stream->_bufsiz == 0 ? BUFSIZ : stream->_bufsiz;
    stream->_ptr = stream->_base;
    return WEOF;
  }

  stream->_ptr = stream->_base;
  if ((stream->_flag & _IOLBF) == 0U) {
    capacity = stream->_bufsiz == 0 ? BUFSIZ : stream->_bufsiz;
    stream->_wcnt = capacity - 1;
    *stream->_ptr++ = (char)byte;
  }
  return (wint_t)byte;

write_error:
  stream->_flag |= _IOERR;
  return WEOF;
}
