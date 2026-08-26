typedef unsigned int size_t;

extern void _near *nmalloc(size_t);

void _near *ncalloc(size_t count, size_t size)
{
  size_t total;
  size_t index;
  unsigned char _near *memory;
  if (count != 0U && size > (size_t)(0xffffU / count))
    return 0;
  total = count * size;
  memory = (unsigned char _near *)nmalloc(total);
  if (memory != 0)
    for (index = 0U; index < total; ++index)
      memory[index] = 0U;
  return memory;
}
