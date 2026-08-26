#include "dc166_stdio.h"

int fputs(const char *string, FILE *stream)
{
  while (*string != 0) {
    if (fputc((int)*string, stream) != (int)*string)
      return EOF;
    ++string;
  }
  return 0;
}
