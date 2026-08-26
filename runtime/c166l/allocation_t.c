typedef unsigned int size_t;

extern void _near *nmalloc(size_t);
extern void _near *ncalloc(size_t, size_t);
extern void _near *nrealloc(void _near *, size_t);
extern void nfree(void _near *);

void *malloc(size_t size) { return nmalloc(size); }
void *calloc(size_t count, size_t size) { return ncalloc(count, size); }
void *realloc(void *memory, size_t size) { return nrealloc(memory, size); }
void free(void *memory) { nfree(memory); }
