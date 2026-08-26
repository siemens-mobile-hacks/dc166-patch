#include "test.h"
#include <stddef.h>
#include <wchar.h>

extern int _strcasecmp(const char *, const char *);
extern int _strncasecmp(const char *, const char *, size_t);
extern int _wcscasecmp(const wchar_t *, const wchar_t *);
extern int _wcsncasecmp(const wchar_t *, const wchar_t *, size_t);


void main(void)
{
  simulator_result = 0U;

  test_check(1U, _strcasecmp("AbC", "aBc") == 0);
  test_check(2U, _strcasecmp("a", "B") == -1);
  test_check(3U, _strcasecmp("C", "a") == 2);
  test_check(4U, _strcasecmp("a", "aa") == -65);
  test_check(5U, _strncasecmp("abX", "ABY", 2U) == 0);
  test_check(6U, _strncasecmp("abX", "ABY", 3U) == -1);
  test_check(7U, _strncasecmp("a", "B", 0U) == 0);

  test_check(8U, _wcscasecmp(L"AbC", L"aBc") == 0);
  test_check(9U, _wcscasecmp(L"a", L"B") == -1);
  test_check(10U, _wcscasecmp(L"C", L"a") == 2);
  test_check(11U, _wcscasecmp(L"a", L"aa") == -65);
  test_check(12U, _wcsncasecmp(L"abX", L"ABY", 2U) == 0);
  test_check(13U, _wcsncasecmp(L"abX", L"ABY", 3U) == -1);
  test_check(14U, _wcsncasecmp(L"a", L"B", 0U) == 0);

  simulator_stop();
}

#ifndef DC166_SMALL_ORACLE
#endif
