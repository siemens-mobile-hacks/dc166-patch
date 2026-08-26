#include "dc166_fss.h"

extern int _open(const char *, int);

static unsigned char initializing;
static unsigned char descriptor_initialized[3];

void _fss_init(int fd, int is_descriptor)
{
  if (initializing)
    return;
  if (is_descriptor && fd >= 0 && fd < 3) {
    descriptor_initialized[fd] = 1U;
    return;
  }

  initializing = 1U;
  if (!descriptor_initialized[0]) {
    descriptor_initialized[0] = 1U;
    (void)_open("", 0);
  }
  if (!descriptor_initialized[1]) {
    descriptor_initialized[1] = 1U;
    (void)_open("", 1);
  }
  if (!descriptor_initialized[2]) {
    descriptor_initialized[2] = 1U;
    (void)_open("", 1);
  }
  initializing = 0U;
}
