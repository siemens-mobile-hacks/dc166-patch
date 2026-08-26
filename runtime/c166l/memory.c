typedef unsigned int size_t;

void *memcpy(void *destination, const void *source, size_t count)
{
  unsigned char *output = (unsigned char *)destination;
  const unsigned char *input = (const unsigned char *)source;

  while (count-- != 0)
    *output++ = *input++;
  return destination;
}

void *memmove(void *destination, const void *source, size_t count)
{
  unsigned char *output = (unsigned char *)destination;
  const unsigned char *input = (const unsigned char *)source;

  if (output < input) {
    while (count-- != 0)
      *output++ = *input++;
  } else if (output != input) {
    output += count;
    input += count;
    while (count-- != 0)
      *--output = *--input;
  }
  return destination;
}

void *memchr(const void *source, int character, size_t count)
{
  const unsigned char *input = (const unsigned char *)source;
  unsigned char value = (unsigned char)character;

  while (count-- != 0) {
    if (*input == value)
      return (void *)input;
    ++input;
  }
  return 0;
}

void *memset(void *destination, int character, size_t count)
{
  unsigned char *output = (unsigned char *)destination;
  unsigned char value = (unsigned char)character;

  while (count-- != 0)
    *output++ = value;
  return destination;
}

int memcmp(const void *left, const void *right, size_t count)
{
  const unsigned char *left_bytes = (const unsigned char *)left;
  const unsigned char *right_bytes = (const unsigned char *)right;

  while (count-- != 0) {
    if (*left_bytes != *right_bytes)
      return (int)*left_bytes - (int)*right_bytes;
    ++left_bytes;
    ++right_bytes;
  }
  return 0;
}
