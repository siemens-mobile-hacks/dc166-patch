#include "test.h"

typedef unsigned int size_t;

extern int errno;
extern int atoi(const char *);
extern long atol(const char *);
extern long strtol(const char *, char **, int);
extern unsigned long strtoul(const char *, char **, int);
extern void *bsearch(const void *, const void *, size_t, size_t,
                     int (*)(const void *, const void *));
extern void qsort(void *, size_t, size_t,
                  int (*)(const void *, const void *));

static int values[10];
static const char no_digits[] = "xyz";

static int compare_int(const void *left, const void *right)
{
  int left_value = *(const int *)left;
  int right_value = *(const int *)right;

  if (left_value < right_value)
    return -1;
  if (left_value > right_value)
    return 1;
  return 0;
}

static void test_conversions(void)
{
  char *end;
  long signed_value;
  unsigned long unsigned_value;

  signed_value = strtol("  -123x", &end, 10);
  test_check(1U, signed_value == -123L && *end == 'x');
  signed_value = strtol("0x2a!", &end, 0);
  test_check(2U, signed_value == 42L && *end == '!');
  signed_value = strtol("0779", &end, 0);
  test_check(3U, signed_value == 63L && *end == '9');
  signed_value = strtol(no_digits, &end, 10);
  test_check(4U, signed_value == 0L && end == no_digits);
  test_check(5U, strtol("2147483647", 0, 10) == 2147483647L);
  test_check(6U, strtol("-2147483648", 0, 10) == (long)0x80000000UL);

  errno = 0;
  signed_value = strtol("2147483648tail", &end, 10);
  test_check(7U, signed_value == 2147483647L && errno == 24 && *end == 't');
  errno = 0;
  signed_value = strtol("-2147483649", &end, 10);
  test_check(8U, signed_value == (long)0x80000000UL && errno == 24 && *end == 0);

  test_check(9U, strtoul("4294967295", 0, 10) == 0xffffffffUL);
  test_check(10U, strtoul("-1", 0, 10) == 0xffffffffUL);
  errno = 0;
  unsigned_value = strtoul("4294967296z", &end, 10);
  test_check(11U, unsigned_value == 0xffffffffUL && errno == 24 && *end == 'z');
  errno = 0;
  unsigned_value = strtoul("123", &end, 1);
  test_check(12U, unsigned_value == 0UL);
  test_check(13U, errno == 13);
  test_check(14U, end[0] == '1');

  test_check(15U, atoi("-321") == -321);
  test_check(16U, atol("1234567") == 1234567L);
}

static void test_algorithms(void)
{
  static const int input[10] = {7, -2, 7, 4, 0, 9, -8, 3, 1, 5};
  static const int expected[10] = {-8, -2, 0, 1, 3, 4, 5, 7, 7, 9};
  int key;
  int *found;
  test_u16 index;

  for (index = 0U; index < 10U; ++index)
    values[index] = input[index];
  qsort(values, 10U, sizeof(values[0]), compare_int);
  for (index = 0U; index < 10U; ++index)
    test_check(17U, values[index] == expected[index]);

  key = 4;
  found = (int *)bsearch(&key, values, 10U, sizeof(values[0]), compare_int);
  test_check(18U, found != 0 && *found == 4);
  key = 6;
  test_check(19U, bsearch(&key, values, 10U,
                          sizeof(values[0]), compare_int) == 0);
  test_check(20U, bsearch(&key, values, 0U,
                          sizeof(values[0]), compare_int) == 0);
}

void main(void)
{
  simulator_result = 0U;
  test_conversions();
  test_algorithms();
  simulator_stop();
}
