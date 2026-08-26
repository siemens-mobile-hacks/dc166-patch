#include "dc166_stdio.h"

#undef putwc

wint_t putwc(wchar_t character, FILE *stream)
{
  return fputwc(character, stream);
}
