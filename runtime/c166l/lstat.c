#include <errno.h>
#include <unistd.h>

int lstat(const char *name, struct stat *destination)
{
  (void)name;
  (void)destination;
  errno = ENOENT;
  return -1;
}
