#include "dc166_stdio.h"

void cleanup(void)
{
  int index;

  for (index = 0; index < _NFILE; ++index)
    fclose(&_iob[index]);
}
