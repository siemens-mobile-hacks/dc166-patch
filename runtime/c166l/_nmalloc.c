typedef unsigned int size_t;

typedef struct near_block {
  size_t span;
  size_t used_size;
} near_block;

extern unsigned char _near *dc166_near_heap_begin(void);
extern unsigned char _near *dc166_near_heap_end(void);

static near_block _near *near_first;
static unsigned char _near *near_end;
static int near_initialized;

static void initialize_near_heap(void)
{
  unsigned char _near *begin;
  size_t available;
  if (near_initialized)
    return;
  near_initialized = 1;
  begin = dc166_near_heap_begin();
  near_end = dc166_near_heap_end();
  available = (size_t)(near_end - begin);
  if (available < sizeof(near_block) + 2U) {
    near_first = 0;
    return;
  }
  near_first = (near_block _near *)begin;
  near_first->span = available;
  near_first->used_size = 0U;
}

static near_block _near *near_next(near_block _near *block)
{
  unsigned char _near *next = (unsigned char _near *)block + block->span;
  return next >= near_end ? 0 : (near_block _near *)next;
}

static void near_merge(near_block _near *block)
{
  near_block _near *next = near_next(block);
  if (next != 0 && next->used_size == 0U)
    block->span += next->span;
}

void _near *nmalloc(size_t requested_size)
{
  near_block _near *block;
  size_t size;
  size_t wanted;
  if (requested_size == 0U)
    return 0;
  size = (requested_size + 1U) & ~1U;
  if (size < requested_size)
    return 0;
  initialize_near_heap();
  block = near_first;
  wanted = sizeof(near_block) + size;
  while (block != 0) {
    if (block->used_size == 0U && block->span >= wanted) {
      size_t remaining = block->span - wanted;
      if (remaining >= sizeof(near_block) + 2U) {
        near_block _near *tail = (near_block _near *)
            ((unsigned char _near *)block + wanted);
        tail->span = remaining;
        tail->used_size = 0U;
        block->span = wanted;
        near_merge(tail);
      }
      block->used_size = requested_size;
      return (void _near *)((unsigned char _near *)block +
                            sizeof(near_block));
    }
    block = near_next(block);
  }
  return 0;
}

void nfree(void _near *memory)
{
  near_block _near *block;
  near_block _near *current;
  if (memory == 0)
    return;
  block = (near_block _near *)((unsigned char _near *)memory -
                               sizeof(near_block));
  block->used_size = 0U;
  near_merge(block);
  current = near_first;
  while (current != 0 && near_next(current) != block)
    current = near_next(current);
  if (current != 0 && current->used_size == 0U)
    near_merge(current);
}

size_t dc166_near_size(void _near *memory)
{
  near_block _near *block = (near_block _near *)
      ((unsigned char _near *)memory - sizeof(near_block));
  return block->used_size;
}
