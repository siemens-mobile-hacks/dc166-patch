#include "dc166_fss.h"
#include <string.h>

char *getcwd(char *destination, unsigned int size)
{
  int result;

  if (size > FSS_MAXBUF)
    size = FSS_MAXBUF;
  _fss_buffer.buff_size = DC166_FSS_PACKET_SIZE(getcwd);
  _fss_buffer.u1.call_id = FSS_GETCWD;
  _fss_buffer.u2.getcwd.size = size;
  _fss_break();
  result = _fss_buffer.u1.retval;
  if (result <= 0) {
    errno = _fss_buffer.u2.errno;
    return 0;
  }
  strcpy(destination, _fss_buffer.u2.getcwd.path);
  return destination;
}
