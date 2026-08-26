#include "dc166_stdio.h"

FILE *tmpfile(void)
{
  static char name[L_tmpnam];

  tmpnam(name);
  return fopen(name, "w+b");
}
