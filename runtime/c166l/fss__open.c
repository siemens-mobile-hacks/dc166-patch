#include "dc166_fss.h"
#include <string.h>

int _open(const char *name, int flags)
{
  unsigned int length;
  int result;

  _fss_init(0, 0);
  length = strlen(name);
  if (length >= FSS_MAXBUF) {
    errno = ENAMETOOLONG;
    return -1;
  }

  _fss_buffer.buff_size = (int)length + 7;
  _fss_buffer.u1.call_id = FSS_OPEN;
  _fss_buffer.u2.open.flags = flags;
  strcpy(_fss_buffer.u2.open.name, name);
  _fss_break();
  result = _fss_buffer.u1.retval;
  DC166_FSS_SET_ERROR(result);
  return result;
}
