#include "test.h"
#include <string.h>
#include <time.h>
#include <wchar.h>

extern int _tzset(const char *);
extern char *_tzget(int);
extern void _stime(time_t *);

static char formatted[160];
static wchar_t wide_formatted[160];

static void test_calendar(void)
{
  time_t stamp = 0UL;
  struct tm *value = gmtime(&stamp);

  test_check(1U, value->tm_sec == 0 && value->tm_min == 0 &&
                 value->tm_hour == 0 && value->tm_mday == 1);
  test_check(2U, value->tm_mon == 0 && value->tm_year == 70 &&
                 value->tm_wday == 4 && value->tm_yday == 0 &&
                 value->tm_isdst == -1);

  stamp = 951827696UL;
  value = gmtime(&stamp);
  test_check(3U, value->tm_year == 100 && value->tm_mon == 1 &&
                 value->tm_mday == 29);
  test_check(4U, value->tm_hour == 12 && value->tm_min == 34 &&
                 value->tm_sec == 56 && value->tm_yday == 59);
}

static void test_timezone_and_mktime(void)
{
  time_t stamp = 0UL;
  struct tm input;
  struct tm *value;

  test_check(5U, strcmp(_tzget(0), "UTC") == 0 &&
                 strcmp(_tzget(1), "UTC") == 0 &&
                 strcmp(_tzget(2), "+0000") == 0);
  test_check(6U, _tzset("EST05EDT") == 1);
  test_check(7U, strcmp(_tzget(0), "EST") == 0 &&
                 strcmp(_tzget(1), "EDT") == 0 &&
                 strcmp(_tzget(2), "+0300") == 0);
  test_check(8U, _tzset("UTC0") == 0 && strcmp(_tzget(0), "EST") == 0);

  value = localtime(&stamp);
  test_check(9U, value->tm_year == 70 && value->tm_mon == 0 &&
                 value->tm_mday == 1 && value->tm_hour == 5 &&
                 value->tm_isdst == -1);

  input.tm_sec = 0;
  input.tm_min = 0;
  input.tm_hour = 5;
  input.tm_mday = 1;
  input.tm_mon = 0;
  input.tm_year = 70;
  input.tm_wday = -99;
  input.tm_yday = -99;
  input.tm_isdst = 77;
  test_check(10U, mktime(&input) == 0UL);
  test_check(11U, input.tm_wday == 4 && input.tm_yday == 0 &&
                  input.tm_isdst == 77);

  input.tm_year = 69;
  input.tm_mon = 12;
  test_check(12U, mktime(&input) == 0UL);
  test_check(13U, input.tm_year == 69 && input.tm_mon == 12);
  input.tm_year = 206;
  input.tm_mon = 0;
  test_check(14U, mktime(&input) == (time_t)-1);
}

static void test_formatting(void)
{
  time_t stamp = 0UL;
  struct tm *value = localtime(&stamp);
  size_t length;

  length = strftime(formatted, sizeof(formatted),
                    "%a %A %b %B %d %H %I %j %m %M %p %S %U %w %W %x %X %y %Y %Z %% %q",
                    value);
  test_check(15U, length != 0U);
  test_check(16U, strcmp(formatted,
      "Thu Thursday Jan January 01 05 05 001 01 00 AM 00 00 4 00 Thu Jan 01, 1970 05:00:00 70 1970 EST % q") == 0);
  test_check(17U, strcmp(asctime(value), "Thu Jan 01 05:00:00 1970\n") == 0);
  test_check(22U, strcmp(ctime(&stamp), "Thu Jan 01 05:00:00 1970\n") == 0);
  test_check(18U, strftime(formatted, 4U, "abcd", value) == 0U);
  test_check(19U, strftime(formatted, 5U, "abcd", value) == 4U &&
                  strcmp(formatted, "abcd") == 0);
}

static void test_wide_formatting(void)
{
  time_t stamp = 0UL;
  struct tm *value = localtime(&stamp);
  size_t length;

  length = wcsftime(wide_formatted, 160U,
      L"%a %A %b %B %d %H %I %j %m %M %p %S %U %w %W %x %X %y %Y %Z %% %q",
      value);
  test_check(23U, length != 0U);
  test_check(24U, wcscmp(wide_formatted,
      L"Thu Thursday Jan January 01 05 05 001 01 00 AM 00 00 4 00 Thu Jan 01, 1970 05:00:00 70 1970 EST % ") == 0);
  test_check(25U, wcsftime(wide_formatted, 4U, L"abcd", value) == 0U);
  test_check(26U, wcsftime(wide_formatted, 5U, L"abcd", value) == 4U &&
                  wcscmp(wide_formatted, L"abcd") == 0);
}

static void test_software_clock(void)
{
  time_t value;

  test_check(20U, time(0) == (time_t)-1);
  value = 1234567UL;
  _stime(&value);
  value = 0UL;
  test_check(21U, time(&value) == 1234567UL && value == 1234567UL);
}

void main(void)
{
  simulator_result = 0U;
  test_calendar();
  test_timezone_and_mktime();
  test_formatting();
  test_wide_formatting();
  test_software_clock();
  simulator_stop();
}
