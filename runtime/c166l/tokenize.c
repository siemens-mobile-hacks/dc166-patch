typedef unsigned int size_t;

extern size_t _fstrspn(const char _far *, const char _far *);
extern char _far *_fstrpbrk(const char _far *, const char _far *);
extern size_t _hstrspn(const char _huge *, const char _huge *);
extern char _huge *_hstrpbrk(const char _huge *, const char _huge *);
extern size_t _sstrspn(const char _shuge *, const char _shuge *);
extern char _shuge *_sstrpbrk(const char _shuge *, const char _shuge *);

static char _far *far_next_token;
static char _huge *huge_next_token;
static char _shuge *shuge_next_token;

char _far *_fstrtok(char _far *source, const char _far *delimiters)
{
  char _far *token;
  char _far *separator;

  if (source == 0)
    source = far_next_token;
  if (source == 0)
    return 0;
  source += _fstrspn(source, delimiters);
  if (*source == 0) {
    far_next_token = 0;
    return 0;
  }
  token = source;
  separator = _fstrpbrk(source, delimiters);
  if (separator == 0)
    far_next_token = 0;
  else {
    *separator = 0;
    far_next_token = separator + 1;
  }
  return token;
}

char _huge *_hstrtok(char _huge *source, const char _huge *delimiters)
{
  char _huge *token;
  char _huge *separator;

  if (source == 0)
    source = huge_next_token;
  if (source == 0)
    return 0;
  source += _hstrspn(source, delimiters);
  if (*source == 0) {
    huge_next_token = 0;
    return 0;
  }
  token = source;
  separator = _hstrpbrk(source, delimiters);
  if (separator == 0)
    huge_next_token = 0;
  else {
    *separator = 0;
    huge_next_token = separator + 1;
  }
  return token;
}

char _shuge *_sstrtok(char _shuge *source, const char _shuge *delimiters)
{
  char _shuge *token;
  char _shuge *separator;

  if (source == 0)
    source = shuge_next_token;
  if (source == 0)
    return 0;
  source += _sstrspn(source, delimiters);
  if (*source == 0) {
    shuge_next_token = 0;
    return 0;
  }
  token = source;
  separator = _sstrpbrk(source, delimiters);
  if (separator == 0)
    shuge_next_token = 0;
  else {
    *separator = 0;
    shuge_next_token = separator + 1;
  }
  return token;
}
