#include "dc166_stdio.h"

int putchar(int character)
{
  unsigned char byte = (unsigned char)character;

  if (--stdout->_cnt < 0)
    return _flsbuf((int)byte, stdout);
  *stdout->_ptr++ = (char)byte;
  return (int)byte;
}
