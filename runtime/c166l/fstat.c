#include <errno.h>
#include <unistd.h>

int fstat(int fd, struct stat *destination)
{
  (void)fd;
  (void)destination;
  errno = ENOENT;
  return -1;
}
