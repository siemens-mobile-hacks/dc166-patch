#include "dc166_fss.h"

long _lseek(int fd, long offset, int whence)
{
  long result;

  _fss_init(fd, 1);
  _fss_buffer.buff_size = DC166_FSS_PACKET_SIZE(lseek.in);
  _fss_buffer.u1.call_id = FSS_LSEEK;
  _fss_buffer.u2.lseek.in.fd = fd;
  _fss_buffer.u2.lseek.in.offset = offset;
  _fss_buffer.u2.lseek.in.whence = whence;
  _fss_break();
  result = _fss_buffer.u2.lseek.out.retval;
  if (result < 0L)
    errno = _fss_buffer.u2.lseek.out.errno;
  return result;
}
