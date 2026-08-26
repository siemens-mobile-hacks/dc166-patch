extern unsigned int _read(int, char *, unsigned int);

unsigned int read(int fd, char *buffer, unsigned int count)
{
  return _read(fd, buffer, count);
}
