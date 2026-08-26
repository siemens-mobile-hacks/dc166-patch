#ifndef DC166_FSS_PRIVATE_H
#define DC166_FSS_PRIVATE_H

#include <errno.h>
#include <fss.h>

extern FSS_BUF _fss_buffer;

void _fss_break(void);
void _fss_init(int fd, int is_descriptor);

#define DC166_FSS_HEADER_SIZE ((int)(sizeof(int) + sizeof(int)))
#define DC166_FSS_PACKET_SIZE(member) \
  ((int)(DC166_FSS_HEADER_SIZE + sizeof(_fss_buffer.u2.member)))

#define DC166_FSS_SET_ERROR(value) \
  ((void)((value) < 0 ? (errno = _fss_buffer.u2.errno) : 0))

#endif
