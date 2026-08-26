#include "dc166_stdio.h"

size_t fread(void *buffer, size_t size, size_t count, FILE *stream)
{
  unsigned char *next = (unsigned char *)buffer;
  size_t item;
  size_t offset;
  int character;

  for (item = 0U; item < count; ++item) {
    for (offset = 0U; offset < size; ++offset) {
      character = fgetc(stream);
      if (character == EOF)
        return item;
      *next++ = (unsigned char)character;
    }
  }
  return item;
}
