#include "dc166_fss.h"

int _close(int fd)
{
  int result;

  _fss_init(fd, 1);
  _fss_buffer.buff_size = DC166_FSS_PACKET_SIZE(close);
  _fss_buffer.u1.call_id = FSS_CLOSE;
  _fss_buffer.u2.close.fd = fd;
  _fss_break();
  result = _fss_buffer.u1.retval;
  DC166_FSS_SET_ERROR(result);
  return result;
}
