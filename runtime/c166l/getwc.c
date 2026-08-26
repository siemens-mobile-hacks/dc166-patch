#include "dc166_stdio.h"

#undef getwc

wint_t getwc(FILE *stream)
{
  return fgetwc(stream);
}
