typedef unsigned int size_t;

void *bsearch(const void *key, const void *base, size_t count, size_t size,
              int (*compare)(const void *, const void *))
{
  const unsigned char *bytes = (const unsigned char *)base;
  size_t first = 0U;

  if (size == 0U)
    return 0;
  while (first < count) {
    size_t middle = first + (count - first) / 2U;
    const void *element = bytes + middle * size;
    int order = compare(key, element);

    if (order < 0)
      count = middle;
    else if (order > 0)
      first = middle + 1U;
    else
      return (void *)element;
  }
  return 0;
}

static void swap_elements(unsigned char *left, unsigned char *right,
                          size_t size)
{
  while (size-- != 0U) {
    unsigned char temporary = *left;
    *left++ = *right;
    *right++ = temporary;
  }
}

void qsort(void *base, size_t count, size_t size,
           int (*compare)(const void *, const void *))
{
  unsigned char *bytes = (unsigned char *)base;
  size_t gap;

  if (size == 0U || count < 2U)
    return;

  for (gap = count / 2U; gap != 0U; gap /= 2U) {
    size_t index;

    for (index = gap; index < count; ++index) {
      size_t current = index;

      while (current >= gap) {
        unsigned char *left = bytes + (current - gap) * size;
        unsigned char *right = bytes + current * size;

        if (compare(left, right) <= 0)
          break;
        swap_elements(left, right, size);
        current -= gap;
      }
    }
  }
}
