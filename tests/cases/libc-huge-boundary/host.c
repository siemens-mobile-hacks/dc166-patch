#include "test.h"

typedef unsigned int size_t;

extern char _huge *_hstrcat(char _huge *, const char _huge *);
extern char _huge *_hstrchr(const char _huge *, int);
extern size_t _hstrcspn(const char _huge *, const char _huge *);
extern size_t _hstrlen(const char _huge *);
extern char _huge *_hstrncat(char _huge *, const char _huge *, size_t);
extern int _hstrncmp(const char _huge *, const char _huge *, size_t);
extern char _huge *_hstrncpy(char _huge *, const char _huge *, size_t);
extern char _huge *_hstrpbrk(const char _huge *, const char _huge *);
extern char _huge *_hstrrchr(const char _huge *, int);
extern size_t _hstrspn(const char _huge *, const char _huge *);
extern char _huge *_hstrstr(const char _huge *, const char _huge *);
extern char _huge *_hstrcpy(char _huge *, const char _huge *);
extern int _hstrcmp(const char _huge *, const char _huge *);


static void put_text(char _huge *destination, const char *source)
{
  while ((*destination++ = *source++) != 0)
    ;
}

void main(void)
{
  char _huge *source = (char _huge *)0x20fffcUL;
  char _huge *destination = (char _huge *)0x21fffcUL;
  char _huge *scratch = (char _huge *)0x220010UL;

  simulator_result = 0U;
  put_text(source, "abca");
  test_check(1U, _hstrlen(source) == 4U);
  test_check(2U, _hstrcpy(destination, source) == destination);
  test_check(3U, _hstrcmp(destination, source) == 0);
  test_check(4U, _hstrcat(destination, (const char _huge *)"Z") == destination);
  test_check(5U, _hstrlen(destination) == 5U);
  test_check(6U, _hstrchr(destination, 'c') == destination + 2U);
  test_check(7U, _hstrrchr(destination, 'a') == destination + 3U);
  test_check(8U, _hstrspn(destination, (const char _huge *)"abc") == 4U);
  test_check(9U, _hstrcspn(destination, (const char _huge *)"Z") == 4U);
  test_check(10U, _hstrpbrk(destination, (const char _huge *)"Z") ==
                  destination + 4U);
  test_check(11U, _hstrstr(destination, (const char _huge *)"ca") ==
                  destination + 2U);
  test_check(12U, _hstrncmp(destination, (const char _huge *)"abcb", 3U) == 0);
  _hstrncpy(scratch, destination, 3U);
  scratch[3] = 0;
  test_check(13U, _hstrcmp(scratch, (const char _huge *)"abc") == 0);
  _hstrcpy(scratch, (const char _huge *)"x");
  _hstrncat(scratch, destination, 2U);
  test_check(14U, _hstrcmp(scratch, (const char _huge *)"xab") == 0);
  simulator_stop();
}
