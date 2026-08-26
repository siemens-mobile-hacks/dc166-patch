#include "dc166_stdio.h"

wint_t fputwc(wchar_t character, FILE *stream)
{
  if ((unsigned int)character >= 0x100U) {
    errno = EILSEQ;
    return WEOF;
  }

  if ((stream->_flag & _IOSTRG) != 0U) {
    if (stream->_cnt-- > 0) {
      *(wchar_t *)stream->_ptr = character;
      stream->_ptr += sizeof(wchar_t);
      return (wint_t)character;
    }
    return WEOF;
  }

  if (--stream->_wcnt >= 0) {
    *stream->_ptr++ = (char)character;
    return (wint_t)(unsigned char)character;
  }
  return _wflsbuf(character, stream);
}
