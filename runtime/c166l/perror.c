#include "dc166_stdio.h"

void perror(const char *prefix)
{
  if (prefix != 0 && *prefix != 0) {
    fputs(prefix, stderr);
    fputs(": ", stderr);
  }
  fputs(strerror(errno), stderr);
  fputc('\n', stderr);
}
