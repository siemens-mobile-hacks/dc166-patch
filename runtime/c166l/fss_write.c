extern unsigned int _write(int, char *, unsigned int);

unsigned int write(int fd, const char *buffer, unsigned int count)
{
  return _write(fd, (char *)buffer, count);
}
