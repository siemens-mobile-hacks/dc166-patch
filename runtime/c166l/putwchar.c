#include "dc166_stdio.h"

#undef putwchar

wint_t putwchar(wchar_t character)
{
  return fputwc(character, stdout);
}
