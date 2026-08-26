#include "dc166_stdio.h"

static char temporary_name[L_tmpnam];
static unsigned int temporary_number;

char *tmpnam(char *name)
{
  unsigned int value;
  int index;

  if (name == 0)
    name = temporary_name;
  ++temporary_number;
  strcpy(name, _P_tmpdir);
  name[8] = 0;
  value = temporary_number;
  for (index = 7; index >= 3; --index) {
    name[index] = (char)('0' + (value & 7U));
    value >>= 3;
  }
  return name;
}
