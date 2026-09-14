typedef unsigned int size_type;
typedef void *(*copy_function)(void *, const void *, size_type);
typedef void *(*set_function)(void *, int, size_type);

extern void *memcpy(void *, const void *, size_type);
extern void *memmove(void *, const void *, size_type);
extern void *memset(void *, int, size_type);

copy_function volatile copy_pointer = memcpy;
copy_function volatile move_pointer = memmove;
set_function volatile set_pointer = memset;
