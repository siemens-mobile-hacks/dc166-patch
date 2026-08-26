#include "dc166_fss.h"
#include <string.h>

unsigned int _read(int fd, char *buffer, unsigned int count)
{
  int result;

  _fss_init(fd, 1);
  if (count <= FSS_MAXBUF) {
    _fss_buffer.buff_size = DC166_FSS_PACKET_SIZE(read_short.in);
    _fss_buffer.u1.call_id = FSS_READ_SHORT;
    _fss_buffer.u2.read_short.in.fd = fd;
    _fss_buffer.u2.read_short.in.count = count;
  } else {
    _fss_buffer.buff_size = DC166_FSS_PACKET_SIZE(read_long);
    _fss_buffer.u1.call_id = FSS_READ_LONG;
    _fss_buffer.u2.read_long.fd = fd;
    _fss_buffer.u2.read_long.count = count;
    _fss_buffer.u2.read_long.buffer = buffer;
  }
  _fss_break();

  result = _fss_buffer.u1.retval;
  if (result < 0)
    errno = _fss_buffer.u2.errno;
  else if (count <= FSS_MAXBUF)
    memcpy(buffer, _fss_buffer.u2.read_short.out.buffer,
           (unsigned int)result);
  return (unsigned int)result;
}
