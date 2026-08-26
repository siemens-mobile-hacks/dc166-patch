#include "dc166_stdio.h"

wint_t _wfilbuf(FILE *stream)
{
  int requested;
  int result = 0;
  unsigned char byte;

  if ((stream->_flag & _IOCHAR) != 0U)
    return WEOF;
  stream->_flag |= _IOWCHAR;
  if (stream == stdin)
    fflush(stdout);
  if ((stream->_flag & _IORD) == 0U ||
      (stream->_flag & _IOSTRG) != 0U)
    return WEOF;

  if ((stream->_flag & _IONBF) == 0U) {
    if (stream->_wcnt > 0) {
      --stream->_wcnt;
      fflush(stream);
    }
    requested = stream->_bufsiz == 0 ? BUFSIZ : stream->_bufsiz;
    stream->_wcnt = 0;
    stream->_ptr = stream->_base;
    if ((stream->_flag & _IOLBF) == 0U) {
      result = (int)_read((int)stream->_file, stream->_base,
                          (unsigned int)requested);
      if (result < 0)
        goto read_error;
      stream->_wcnt = -result;
    } else {
      result = 0;
      while (requested-- > 0) {
        result = (int)_read((int)stream->_file, (char *)&byte, 1U);
        if (result != 1)
          break;
        --stream->_wcnt;
        *stream->_ptr++ = (char)byte;
        if (byte == (unsigned char)'\n')
          break;
      }
      stream->_ptr = stream->_base;
    }
    if (stream->_wcnt != 0)
      return (wint_t)(unsigned char)*stream->_ptr++;
  } else {
    stream->_wcnt = 0;
    if (stream->_bufsiz != -1) {
      result = stream->_bufsiz;
      stream->_bufsiz = -1;
      return (wint_t)(unsigned int)result;
    }
    result = (int)_read((int)stream->_file, (char *)&byte, 1U);
    if (result == 1)
      return (wint_t)byte;
  }

  if (result != 0) {
read_error:
    stream->_flag |= _IOERR;
    return WEOF;
  }
  stream->_flag |= _IOEOF;
  return WEOF;
}
