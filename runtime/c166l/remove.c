#include "dc166_stdio.h"

int remove(const char *name)
{
  return _unlink(name);
}
