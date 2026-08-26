typedef unsigned int size_t;

#if _MODEL == 'm' || _MODEL == 'l'
extern char _far *_fstrcpy(char _far *, const char _far *);
extern char _far *_fstrncpy(char _far *, const char _far *, size_t);
extern char _far *_fstrcat(char _far *, const char _far *);
extern char _far *_fstrncat(char _far *, const char _far *, size_t);
extern char _far *_fstrchr(const char _far *, int);
extern char _far *_fstrrchr(const char _far *, int);
extern size_t _fstrspn(const char _far *, const char _far *);
extern size_t _fstrcspn(const char _far *, const char _far *);
extern char _far *_fstrpbrk(const char _far *, const char _far *);
extern char _far *_fstrstr(const char _far *, const char _far *);
extern int _fstrcmp(const char _far *, const char _far *);
extern int _fstrncmp(const char _far *, const char _far *, size_t);
extern size_t _fstrlen(const char _far *);
#endif
extern int toupper(int);

#if _MODEL == 't' || _MODEL == 'h'
char *strcpy(char *destination, const char *source)
{
  char *result = destination;
  while ((*destination++ = *source++) != 0) {}
  return result;
}

char *strncpy(char *destination, const char *source, size_t count)
{
  char *result = destination;
  while (count != 0U && *source != 0) {
    *destination++ = *source++;
    --count;
  }
  while (count-- != 0U)
    *destination++ = 0;
  return result;
}

char *strcat(char *destination, const char *source)
{
  char *result = destination;
  while (*destination != 0)
    ++destination;
  strcpy(destination, source);
  return result;
}

char *strncat(char *destination, const char *source, size_t count)
{
  char *result = destination;
  while (*destination != 0)
    ++destination;
  while (count-- != 0U && *source != 0)
    *destination++ = *source++;
  *destination = 0;
  return result;
}

char *strchr(const char *source, int character)
{
  do {
    if (*source == (char)character)
      return (char *)source;
  } while (*source++ != 0);
  return 0;
}

char *strrchr(const char *source, int character)
{
  const char *result = 0;
  do {
    if (*source == (char)character)
      result = source;
  } while (*source++ != 0);
  return (char *)result;
}

size_t strspn(const char *source, const char *accept)
{
  size_t count = 0U;
  while (*source != 0) {
    const char *candidate = accept;
    while (*candidate != 0 && *candidate != *source)
      ++candidate;
    if (*candidate == 0)
      break;
    ++source;
    ++count;
  }
  return count;
}

size_t strcspn(const char *source, const char *reject)
{
  size_t count = 0U;
  while (*source != 0) {
    const char *candidate = reject;
    while (*candidate != 0 && *candidate != *source)
      ++candidate;
    if (*candidate != 0)
      break;
    ++source;
    ++count;
  }
  return count;
}

char *strpbrk(const char *source, const char *accept)
{
  while (*source != 0) {
    const char *candidate = accept;
    while (*candidate != 0)
      if (*candidate++ == *source)
        return (char *)source;
    ++source;
  }
  return 0;
}

char *strstr(const char *source, const char *needle)
{
  if (*needle == 0)
    return (char *)source;
  while (*source != 0) {
    const char *left = source;
    const char *right = needle;
    while (*right != 0 && *left == *right) {
      ++left;
      ++right;
    }
    if (*right == 0)
      return (char *)source;
    ++source;
  }
  return 0;
}

int strcmp(const char *left, const char *right)
{
  while (*left != 0 && *left == *right) {
    ++left;
    ++right;
  }
  return (int)(unsigned char)*left - (int)(unsigned char)*right;
}

int strncmp(const char *left, const char *right, size_t count)
{
  while (count-- != 0U) {
    int difference =
        (int)(unsigned char)*left - (int)(unsigned char)*right;
    if (difference != 0 || *left == 0)
      return difference;
    ++left;
    ++right;
  }
  return 0;
}

int _strcasecmp(const char *left, const char *right)
{
  unsigned char left_value;
  unsigned char right_value;
  do {
    left_value = (unsigned char)toupper((int)(signed char)*left++);
    right_value = (unsigned char)toupper((int)(signed char)*right++);
  } while (left_value != 0U && left_value == right_value);
  return (int)left_value - (int)right_value;
}

int _strncasecmp(const char *left, const char *right, size_t count)
{
  while (count-- != 0U) {
    unsigned char left_value =
        (unsigned char)toupper((int)(signed char)*left++);
    unsigned char right_value =
        (unsigned char)toupper((int)(signed char)*right++);
    if (left_value == 0U || left_value != right_value)
      return (int)left_value - (int)right_value;
  }
  return 0;
}

size_t strlen(const char *source)
{
  const char *end = source;
  while (*end != 0)
    ++end;
  return (size_t)(end - source);
}

int strcoll(const char *left, const char *right)
{
  return strcmp(left, right);
}

size_t strxfrm(char *destination, const char *source, size_t count)
{
  size_t length = strlen(source);
  size_t index;
  for (index = 0U; index < count && index <= length; ++index)
    destination[index] = source[index];
  return length;
}
#else
char *strcpy(char *destination, const char *source)
{
  return _fstrcpy(destination, source);
}

char *strncpy(char *destination, const char *source, size_t count)
{
  return _fstrncpy(destination, source, count);
}

char *strcat(char *destination, const char *source)
{
  return _fstrcat(destination, source);
}

char *strncat(char *destination, const char *source, size_t count)
{
  return _fstrncat(destination, source, count);
}

char *strchr(const char *source, int character)
{
  return _fstrchr(source, character);
}

char *strrchr(const char *source, int character)
{
  return _fstrrchr(source, character);
}

size_t strspn(const char *source, const char *accept)
{
  return _fstrspn(source, accept);
}

size_t strcspn(const char *source, const char *reject)
{
  return _fstrcspn(source, reject);
}

char *strpbrk(const char *source, const char *accept)
{
  return _fstrpbrk(source, accept);
}

char *strstr(const char *source, const char *needle)
{
  return _fstrstr(source, needle);
}

int strcmp(const char *left, const char *right)
{
  return _fstrcmp(left, right);
}

int strncmp(const char *left, const char *right, size_t count)
{
  return _fstrncmp(left, right, count);
}

int _strcasecmp(const char *left, const char *right)
{
  unsigned char left_value;
  unsigned char right_value;

  do {
    left_value = (unsigned char)toupper((int)(signed char)*left++);
    right_value = (unsigned char)toupper((int)(signed char)*right++);
  } while (left_value != 0U && left_value == right_value);
  return (int)left_value - (int)right_value;
}

int _strncasecmp(const char *left, const char *right, size_t count)
{
  unsigned char left_value;
  unsigned char right_value;

  while (count-- != 0U) {
    left_value = (unsigned char)toupper((int)(signed char)*left++);
    right_value = (unsigned char)toupper((int)(signed char)*right++);
    if (left_value == 0U || left_value != right_value)
      return (int)left_value - (int)right_value;
  }
  return 0;
}

size_t strlen(const char *source)
{
  return _fstrlen(source);
}

int strcoll(const char *left, const char *right)
{
  return _fstrcmp(left, right);
}

size_t strxfrm(char *destination, const char *source, size_t count)
{
  size_t length = _fstrlen(source);
  size_t index;

  if (length < count) {
    for (index = 0; index <= length; ++index)
      destination[index] = source[index];
  } else {
    for (index = 0; index < count; ++index)
      destination[index] = source[index];
  }
  return length;
}
#endif

static char *next_token;

char *strtok(char *source, const char *delimiters)
{
  char *token;
  char *separator;

  if (source == 0)
    source = next_token;
  if (source == 0)
    return 0;

  source += strspn(source, delimiters);
  if (*source == 0) {
    next_token = 0;
    return 0;
  }

  token = source;
  separator = strpbrk(source, delimiters);
  if (separator == 0) {
    next_token = 0;
  } else {
    *separator = 0;
    next_token = separator + 1;
  }
  return token;
}
