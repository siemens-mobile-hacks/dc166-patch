#include "dc166_stdio.h"

static char stdin_buffer[80];
static char stdout_buffer[80];

FILE _iob[_NFILE] = {
  { stdin_buffer, stdin_buffer, 0, 80, _IORD | _IOLBF, 0, 0 },
  { stdout_buffer, stdout_buffer, 0, 80, _IOWR | _IOLBF, 1, 0 },
  { 0, 0, 0, -1, _IOWR | _IONBF, 2, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 }
};

void _doclose(void)
{
  int index;
  for (index = 0; index < _NFILE; ++index)
    if (_iob[index]._flag != 0U)
      fclose(&_iob[index]);
}
