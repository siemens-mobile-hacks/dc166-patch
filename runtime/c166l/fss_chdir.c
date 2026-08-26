#include "dc166_fss.h"
#include <string.h>

int chdir(const char *name)
{
  unsigned int length = strlen(name);
  int result;

  if (length >= FSS_MAXBUF) {
    errno = ENAMETOOLONG;
    return -1;
  }
  _fss_buffer.buff_size = (int)length + 5;
  _fss_buffer.u1.call_id = FSS_CHDIR;
  strcpy(_fss_buffer.u2.chdir.name, name);
  _fss_break();
  result = _fss_buffer.u1.retval;
  DC166_FSS_SET_ERROR(result);
  return result;
}
