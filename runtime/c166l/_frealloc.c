typedef unsigned int size_t;

extern void dc166_hfree(void _huge *);
extern void _huge *dc166_hrealloc(void _huge *, unsigned long, int);

void ffree(void _far *memory)
{
  dc166_hfree((void _huge *)memory);
}

void sfree(void _shuge *memory)
{
  dc166_hfree((void _huge *)memory);
}

void hfree(void _huge *memory)
{
  dc166_hfree(memory);
}

void _far *frealloc(void _far *memory, size_t size)
{
  return (void _far *)dc166_hrealloc((void _huge *)memory,
                                     (unsigned long)size, 1);
}

void _shuge *srealloc(void _shuge *memory, size_t size)
{
  return (void _shuge *)dc166_hrealloc((void _huge *)memory,
                                       (unsigned long)size, 0);
}

void _huge *hrealloc(void _huge *memory, unsigned long size)
{
  return dc166_hrealloc(memory, size, 0);
}
