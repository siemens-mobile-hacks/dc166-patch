#include "dc166_stdio.h"

int _filbuf(FILE *stream)
{
  int requested;
  int result;
  unsigned char byte;

  if ((stream->_flag & _IOWCHAR) != 0U)
    return EOF;
  stream->_flag |= _IOCHAR;
  if (stream == stdin)
    fflush(stdout);
  if ((stream->_flag & _IORD) == 0U ||
      (stream->_flag & _IOSTRG) != 0U)
    return EOF;

  if ((stream->_flag & _IONBF) == 0U) {
    if (stream->_cnt > 0) {
      --stream->_cnt;
      fflush(stream);
    }
    requested = stream->_bufsiz == 0 ? BUFSIZ : stream->_bufsiz;
    stream->_cnt = 0;
    stream->_ptr = stream->_base;
    if ((stream->_flag & _IOLBF) == 0U) {
      result = (int)_read((int)stream->_file, stream->_base,
                          (unsigned int)requested);
      if (result < 0)
        goto read_error;
      stream->_cnt = -result;
    } else {
      result = 0;
      while (requested > 0) {
        result = (int)_read((int)stream->_file, (char *)&byte, 1U);
        if (result != 1)
          break;
        --stream->_cnt;
        *stream->_ptr++ = (char)byte;
        --requested;
        if (byte == (unsigned char)'\n')
          break;
      }
      stream->_ptr = stream->_base;
    }
    stream->_ptr = stream->_base;
    if (stream->_cnt != 0) {
      byte = (unsigned char)*stream->_ptr++;
      return (int)byte;
    }
  } else {
    stream->_cnt = 0;
    if (stream->_bufsiz != -1) {
      result = stream->_bufsiz;
      stream->_bufsiz = -1;
      return result;
    }
    result = (int)_read((int)stream->_file, (char *)&byte, 1U);
    if (result == 1)
      return (int)byte;
  }

  if (result != 0) {
read_error:
    stream->_flag |= _IOERR;
    return EOF;
  }
  stream->_flag |= _IOEOF;
  return EOF;
}
