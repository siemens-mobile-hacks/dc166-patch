#include "dc166_fss.h"
#include <string.h>
#include <unistd.h>

int stat(const char *name, struct stat *destination)
{
  unsigned int length = strlen(name);
  int result;

  if (length >= FSS_MAXBUF) {
    errno = ENAMETOOLONG;
    return -1;
  }
  _fss_buffer.buff_size = (int)length + 5;
  _fss_buffer.u1.call_id = FSS_STAT;
  strcpy(_fss_buffer.u2.stat.name, name);
  _fss_break();
  result = _fss_buffer.u1.retval;
  if (result < 0) {
    errno = _fss_buffer.u2.stat.out.errno;
    return result;
  }

  destination->st_dev = _fss_buffer.u2.stat.out.st_dev;
  destination->st_mode = _fss_buffer.u2.stat.out.st_mode;
  destination->st_nlink = _fss_buffer.u2.stat.out.st_nlink;
  destination->st_size = _fss_buffer.u2.stat.out.st_size;
  destination->st_atime = _fss_buffer.u2.stat.out.st_atime;
  destination->st_mtime = _fss_buffer.u2.stat.out.st_mtime;
  destination->st_ctime = _fss_buffer.u2.stat.out.st_ctime;
  return result;
}
