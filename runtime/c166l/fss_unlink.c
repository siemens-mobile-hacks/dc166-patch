extern int _unlink(const char *);

int unlink(const char *name)
{
  return _unlink(name);
}
