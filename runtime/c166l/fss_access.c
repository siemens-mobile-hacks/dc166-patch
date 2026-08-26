#include "dc166_fss.h"
#include <string.h>

int access(const char *name, int mode)
{
  unsigned int length = strlen(name);
  int result;

  if (length >= FSS_MAXBUF) {
    errno = ENAMETOOLONG;
    return -1;
  }
  _fss_buffer.buff_size = (int)length + 7;
  _fss_buffer.u1.call_id = FSS_ACCESS;
  _fss_buffer.u2.access.mode = mode;
  strcpy(_fss_buffer.u2.access.name, name);
  _fss_break();
  result = _fss_buffer.u1.retval;
  DC166_FSS_SET_ERROR(result);
  return result;
}
