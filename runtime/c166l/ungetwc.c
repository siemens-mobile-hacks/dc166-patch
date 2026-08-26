#include "dc166_stdio.h"

wint_t ungetwc(wint_t character, FILE *stream)
{
  if (character == WEOF || (stream->_flag & _IORD) == 0U)
    return WEOF;
  stream->_flag &= (unsigned short)~_IOEOF;

  if ((stream->_flag & _IOSTRG) != 0U) {
    if (stream->_ptr <= stream->_base)
      return WEOF;
    stream->_ptr -= sizeof(wchar_t);
    --stream->_cnt;
    return character;
  }

  if ((stream->_flag & _IONBF) == 0U) {
    if (stream->_wcnt >= 0) {
      fflush(stream);
      stream->_ptr = stream->_base + stream->_bufsiz;
      stream->_wcnt = -1;
    }
    --stream->_wcnt;
    *--stream->_ptr = (char)character;
    return character;
  }

  if (stream->_bufsiz != -1)
    return WEOF;
  stream->_bufsiz = (int)(character & 0xffU);
  return character;
}
