#include "dc166_stdio.h"

size_t fwrite(const void *buffer, size_t size, size_t count, FILE *stream)
{
  const unsigned char *next = (const unsigned char *)buffer;
  size_t item;
  size_t offset;

  for (item = 0U; item < count; ++item) {
    for (offset = 0U; offset < size; ++offset)
      fputc((int)*next++, stream);
    if ((stream->_flag & _IOERR) != 0U)
      return item;
  }
  return item;
}
