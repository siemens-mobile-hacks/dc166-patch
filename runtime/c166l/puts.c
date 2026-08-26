#include "dc166_stdio.h"

int puts(const char *string)
{
  if (fputs(string, stdout) == EOF || fputc('\n', stdout) != '\n')
    return EOF;
  return 0;
}
