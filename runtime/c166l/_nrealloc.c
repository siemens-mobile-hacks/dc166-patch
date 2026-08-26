typedef unsigned int size_t;

extern void _near *nmalloc(size_t);
extern void nfree(void _near *);
extern size_t dc166_near_size(void _near *);

void _near *nrealloc(void _near *memory, size_t requested_size)
{
  unsigned char _near *replacement;
  size_t old_size;
  size_t copy_size;
  size_t index;
  if (memory == 0)
    return nmalloc(requested_size);
  if (requested_size == 0U) {
    nfree(memory);
    return 0;
  }
  old_size = dc166_near_size(memory);
  if (requested_size <= old_size)
    return memory;
  replacement = (unsigned char _near *)nmalloc(requested_size);
  if (replacement == 0)
    return 0;
  copy_size = old_size < requested_size ? old_size : requested_size;
  for (index = 0U; index < copy_size; ++index)
    replacement[index] = ((unsigned char _near *)memory)[index];
  nfree(memory);
  return replacement;
}
