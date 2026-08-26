#include "dc166_stdio.h"

int fputws(const wchar_t *string, FILE *stream)
{
  while (*string != 0) {
    if (fputwc(*string++, stream) == WEOF)
      return -1;
  }
  return 0;
}
