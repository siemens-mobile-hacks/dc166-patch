typedef unsigned int size_t;

#define HEAP_PAGE_SIZE 0x4000UL
#define HEAP_PAGE_MASK 0x3fffUL

typedef struct heap_block {
  unsigned long span;
  unsigned long used_size;
} heap_block;

extern unsigned char _huge *dc166_heap_begin(void);
extern unsigned char _huge *dc166_heap_end(void);
void *malloc(size_t);
void free(void *);

static heap_block _huge *first_block;
static unsigned char _huge *heap_end;
static int heap_initialized;

static unsigned long address_of(const void _huge *pointer)
{
  return (unsigned long)pointer;
}

static unsigned long aligned_size(unsigned long size)
{
  return (size + 1UL) & ~1UL;
}

static void initialize_heap(void)
{
  unsigned char _huge *begin;
  unsigned long available;

  if (heap_initialized)
    return;
  heap_initialized = 1;
  begin = dc166_heap_begin();
  heap_end = dc166_heap_end();
  available = address_of(heap_end) - address_of(begin);
  if (available < (unsigned long)sizeof(heap_block) + 2UL) {
    first_block = 0;
    return;
  }
  first_block = (heap_block _huge *)begin;
  first_block->span = available;
  first_block->used_size = 0U;
}

static heap_block _huge *next_block(heap_block _huge *block)
{
  unsigned char _huge *next =
      (unsigned char _huge *)block + block->span;

  if (next >= heap_end)
    return 0;
  return (heap_block _huge *)next;
}

static void merge_next(heap_block _huge *block)
{
  heap_block _huge *next = next_block(block);

  if (next != 0 && next->used_size == 0U)
    block->span += next->span;
}

static heap_block _huge *find_previous(heap_block _huge *target)
{
  heap_block _huge *block = first_block;

  while (block != 0) {
    heap_block _huge *next = next_block(block);
    if (next == target)
      return block;
    block = next;
  }
  return 0;
}

static heap_block _huge *place_block(heap_block _huge *free_block,
                                     unsigned long size, int page_limited)
{
  unsigned long block_address = address_of(free_block);
  unsigned long payload_address = block_address + sizeof(heap_block);
  unsigned long block_end = block_address + free_block->span;
  unsigned long allocation_address = block_address;
  unsigned long prefix;
  heap_block _huge *allocation;

  if (page_limited &&
      (payload_address & HEAP_PAGE_MASK) + size > HEAP_PAGE_SIZE) {
    allocation_address =
        (block_address + HEAP_PAGE_SIZE) & ~HEAP_PAGE_MASK;
    prefix = allocation_address - block_address;
    if (prefix < sizeof(heap_block)) {
      allocation_address += HEAP_PAGE_SIZE;
      prefix = allocation_address - block_address;
    }
  } else {
    prefix = 0UL;
  }

  if (allocation_address + sizeof(heap_block) + size > block_end)
    return 0;

  if (prefix != 0UL) {
    free_block->span = prefix;
    free_block->used_size = 0U;
    allocation = (heap_block _huge *)
        ((unsigned char _huge *)free_block + prefix);
    allocation->span = block_end - allocation_address;
  } else {
    allocation = free_block;
  }
  return allocation;
}

static void split_block(heap_block _huge *block, unsigned long size)
{
  unsigned long wanted = sizeof(heap_block) + size;
  unsigned long remaining = block->span - wanted;

  if (remaining >= sizeof(heap_block) + 2UL) {
    heap_block _huge *tail = (heap_block _huge *)
        ((unsigned char _huge *)block + wanted);
    tail->span = remaining;
    tail->used_size = 0U;
    block->span = wanted;
    merge_next(tail);
  }
}

void *calloc(size_t count, size_t size)
{
  size_t total;
  unsigned char *memory;
  size_t index;

  if (count != 0U && size > (size_t)(0xffffU / count))
    return 0;
  total = count * size;
  memory = (unsigned char *)malloc(total);
  if (memory != 0)
    for (index = 0U; index < total; ++index)
      memory[index] = 0U;
  return memory;
}

void _huge *dc166_hmalloc(unsigned long requested_size, int page_limited)
{
  heap_block _huge *block;
  unsigned long size;

  if (requested_size == 0U)
    return 0;
  size = aligned_size(requested_size);
  if (page_limited && size + sizeof(heap_block) > HEAP_PAGE_SIZE)
    return 0;

  initialize_heap();
  block = first_block;
  while (block != 0) {
    if (block->used_size == 0U) {
      heap_block _huge *allocation = place_block(block, size, page_limited);
      if (allocation != 0) {
        split_block(allocation, size);
        allocation->used_size = requested_size;
        return (void _huge *)((unsigned char _huge *)allocation +
                              sizeof(heap_block));
      }
    }
    block = next_block(block);
  }
  return 0;
}

void dc166_hfree(void _huge *memory)
{
  unsigned char _huge *payload;
  heap_block _huge *block;
  heap_block _huge *previous;

  if (memory == 0)
    return;
  payload = (unsigned char _huge *)memory;
  block = (heap_block _huge *)(payload - sizeof(heap_block));
  block->used_size = 0U;
  merge_next(block);
  previous = find_previous(block);
  if (previous != 0 && previous->used_size == 0U)
    merge_next(previous);
}

void _huge *dc166_hrealloc(void _huge *memory, unsigned long requested_size,
                           int page_limited)
{
  unsigned char _huge *payload;
  heap_block _huge *block;
  unsigned long size;
  unsigned long capacity;
  heap_block _huge *next;
  void _huge *replacement;
  unsigned char _huge *destination;
  unsigned char _huge *source;
  unsigned long copy_size;
  unsigned long index;

  if (memory == 0)
    return dc166_hmalloc(requested_size, page_limited);
  if (requested_size == 0U) {
    dc166_hfree(memory);
    return 0;
  }

  payload = (unsigned char _huge *)memory;
  block = (heap_block _huge *)(payload - sizeof(heap_block));
  size = aligned_size(requested_size);
  capacity = block->span - sizeof(heap_block);
  if (size <= capacity) {
    block->used_size = requested_size;
    split_block(block, size);
    return memory;
  }

  next = next_block(block);
  if (next != 0 && next->used_size == 0U &&
      block->span + next->span >= sizeof(heap_block) + size &&
      (!page_limited ||
       (address_of(payload) & HEAP_PAGE_MASK) + size <= HEAP_PAGE_SIZE)) {
    block->span += next->span;
    split_block(block, size);
    block->used_size = requested_size;
    return memory;
  }

  replacement = dc166_hmalloc(requested_size, page_limited);
  if (replacement == 0)
    return 0;
  copy_size = block->used_size < requested_size ?
      block->used_size : requested_size;
  destination = (unsigned char _huge *)replacement;
  source = (unsigned char _huge *)memory;
  for (index = 0U; index < copy_size; ++index)
    destination[index] = source[index];
  dc166_hfree(memory);
  return replacement;
}

void *malloc(size_t requested_size)
{
  return (void *)dc166_hmalloc((unsigned long)requested_size,
                               _MODEL == 'h' ? 0 : 1);
}

void free(void *memory)
{
  dc166_hfree((void _huge *)memory);
}

void *realloc(void *memory, size_t requested_size)
{
  return (void *)dc166_hrealloc((void _huge *)memory,
                                (unsigned long)requested_size,
                                _MODEL == 'h' ? 0 : 1);
}
