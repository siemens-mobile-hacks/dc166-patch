typedef unsigned int size_t;

#define ULONG_MAX_VALUE 0xffffffffUL

extern void _huge *dc166_hmalloc(unsigned long, int);

void _far *fmalloc(size_t size)
{
  return (void _far *)dc166_hmalloc((unsigned long)size, 1);
}

void _shuge *smalloc(size_t size)
{
  return (void _shuge *)dc166_hmalloc((unsigned long)size, 0);
}

void _huge *hmalloc(unsigned long size)
{
  return dc166_hmalloc(size, 0);
}

void _far *fcalloc(size_t count, size_t size)
{
  size_t total;
  size_t index;
  unsigned char _far *memory;
  if (count != 0U && size > (size_t)(0xffffU / count))
    return 0;
  total = count * size;
  memory = (unsigned char _far *)fmalloc(total);
  if (memory != 0)
    for (index = 0U; index < total; ++index)
      memory[index] = 0U;
  return memory;
}

void _shuge *scalloc(size_t count, size_t size)
{
  size_t total;
  size_t index;
  unsigned char _shuge *memory;
  if (count != 0U && size > (size_t)(0xffffU / count))
    return 0;
  total = count * size;
  memory = (unsigned char _shuge *)smalloc(total);
  if (memory != 0)
    for (index = 0U; index < total; ++index)
      memory[index] = 0U;
  return memory;
}

void _huge *hcalloc(unsigned long count, unsigned long size)
{
  unsigned long total;
  unsigned long index;
  unsigned char _huge *memory;
  if (count != 0UL && size > ULONG_MAX_VALUE / count)
    return 0;
  total = count * size;
  memory = (unsigned char _huge *)hmalloc(total);
  if (memory != 0)
    for (index = 0UL; index < total; ++index)
      memory[index] = 0U;
  return memory;
}
