#include "dc166_stdio.h"

#undef getwchar

wint_t getwchar(void)
{
  return fgetwc(stdin);
}
