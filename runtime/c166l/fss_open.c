extern int _open(const char *, int);

int open(const char *name, int flags)
{
  return _open(name, flags);
}
