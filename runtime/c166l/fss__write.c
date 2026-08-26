#include "dc166_fss.h"
#include <string.h>

unsigned int _write(int fd, char *buffer, unsigned int count)
{
  int result;

  _fss_init(fd, 1);
  if (count <= FSS_MAXBUF) {
    _fss_buffer.buff_size = (int)count + 6;
    _fss_buffer.u1.call_id = FSS_WRITE_SHORT;
    _fss_buffer.u2.write_short.fd = fd;
    memcpy(_fss_buffer.u2.write_short.buffer, buffer, count);
  } else {
    _fss_buffer.buff_size = DC166_FSS_PACKET_SIZE(write_long);
    _fss_buffer.u1.call_id = FSS_WRITE_LONG;
    _fss_buffer.u2.write_long.fd = fd;
    _fss_buffer.u2.write_long.count = count;
    _fss_buffer.u2.write_long.buffer = buffer;
  }
  _fss_break();

  result = _fss_buffer.u1.retval;
  DC166_FSS_SET_ERROR(result);
  return (unsigned int)result;
}
