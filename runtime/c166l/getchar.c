#include "dc166_stdio.h"

int getchar(void)
{
  if (++stdin->_cnt < 0)
    return (int)(unsigned char)*stdin->_ptr++;
  return _filbuf(stdin);
}
