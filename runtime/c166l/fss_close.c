extern int _close(int);

int close(int fd)
{
  return _close(fd);
}
