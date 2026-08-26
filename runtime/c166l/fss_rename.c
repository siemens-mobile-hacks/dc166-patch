#include "dc166_fss.h"
#include <string.h>

int rename(const char *old_name, const char *new_name)
{
  int result;

  _fss_buffer.buff_size = DC166_FSS_PACKET_SIZE(rename);
  _fss_buffer.u1.call_id = FSS_RENAME;
  _fss_buffer.u2.rename.old = (char *)old_name;
  _fss_buffer.u2.rename.oldlen = strlen(old_name);
  _fss_buffer.u2.rename.new = (char *)new_name;
  _fss_buffer.u2.rename.newlen = strlen(new_name);
  _fss_break();
  result = _fss_buffer.u1.retval;
  DC166_FSS_SET_ERROR(result);
  return result;
}
