extern long _lseek(int, long, int);

long lseek(int fd, long offset, int whence)
{
  return _lseek(fd, offset, whence);
}
