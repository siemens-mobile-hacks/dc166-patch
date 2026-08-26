#include "dc166_stdio.h"

wint_t fgetwc(FILE *stream)
{
  if ((stream->_flag & _IOSTRG) != 0U) {
    wchar_t value;
    if (++stream->_cnt >= 0)
      return WEOF;
    value = *(wchar_t *)stream->_ptr;
    stream->_ptr += sizeof(wchar_t);
    if ((unsigned int)value >= 0x100U) {
      errno = EILSEQ;
      return WEOF;
    }
    return (wint_t)value;
  }

  if (++stream->_wcnt < 0)
    return (wint_t)(unsigned char)*stream->_ptr++;
  return _wfilbuf(stream);
}
