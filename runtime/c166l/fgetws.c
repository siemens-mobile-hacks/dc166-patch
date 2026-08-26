#include "dc166_stdio.h"

wchar_t *fgetws(wchar_t *string, int count, FILE *stream)
{
  wchar_t *current = string;
  wint_t value = WEOF;

  while (count > 1 && (value = fgetwc(stream)) != WEOF) {
    *current = (wchar_t)value;
    if (value == (wint_t)'\n')
      break;
    ++current;
    --count;
  }
  if (value == WEOF && current == string)
    return 0;
  *current = 0;
  return string;
}
