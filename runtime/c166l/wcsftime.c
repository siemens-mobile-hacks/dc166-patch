#include <wchar.h>

extern char *_tzget(int);

typedef struct {
  wchar_t *destination;
  size_t maximum;
  size_t length;
} wide_time_output;

static const wchar_t *const short_weekdays[7] = {
  L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"
};
static const wchar_t *const long_weekdays[7] = {
  L"Sunday", L"Monday", L"Tuesday", L"Wednesday",
  L"Thursday", L"Friday", L"Saturday"
};
static const wchar_t *const short_months[12] = {
  L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
  L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"
};
static const wchar_t *const long_months[12] = {
  L"January", L"February", L"March", L"April", L"May", L"June",
  L"July", L"August", L"September", L"October", L"November", L"December"
};

static void wide_time_character(wide_time_output *output, wchar_t character)
{
  if (output->length < output->maximum)
    output->destination[output->length] = character;
  ++output->length;
}

static void wide_time_text(wide_time_output *output, const wchar_t *text)
{
  while (*text != 0)
    wide_time_character(output, *text++);
}

static void wide_time_bytes(wide_time_output *output, const char *text)
{
  while (text != 0 && *text != 0)
    wide_time_character(output, (wchar_t)(unsigned char)*text++);
}

static void wide_time_number(wide_time_output *output, int value, int digits)
{
  wchar_t buffer[6];
  int index = digits;
  if (value < 0)
    value = 0;
  while (index != 0) {
    buffer[--index] = (wchar_t)('0' + value % 10);
    value /= 10;
  }
  for (index = 0; index != digits; ++index)
    wide_time_character(output, buffer[index]);
}

static void wide_format_time(wide_time_output *output, const wchar_t *format,
                             const struct tm *value)
{
  while (*format != 0) {
    wchar_t specifier;
    int hour;
    int week;
    if (*format != '%') {
      wide_time_character(output, *format++);
      continue;
    }
    ++format;
    if (*format == 0)
      break;
    specifier = *format++;
    switch (specifier) {
    case 'a': wide_time_text(output, short_weekdays[value->tm_wday]); break;
    case 'A': wide_time_text(output, long_weekdays[value->tm_wday]); break;
    case 'b': wide_time_text(output, short_months[value->tm_mon]); break;
    case 'B': wide_time_text(output, long_months[value->tm_mon]); break;
    case 'c':
      wide_format_time(output, L"%a %b %d %H:%M:%S %Y", value);
      break;
    case 'd': wide_time_number(output, value->tm_mday, 2); break;
    case 'H': wide_time_number(output, value->tm_hour, 2); break;
    case 'I':
      hour = value->tm_hour < 13 ? value->tm_hour : value->tm_hour - 12;
      wide_time_number(output, hour == 0 ? 12 : hour, 2);
      break;
    case 'j': wide_time_number(output, value->tm_yday + 1, 3); break;
    case 'm': wide_time_number(output, value->tm_mon + 1, 2); break;
    case 'M': wide_time_number(output, value->tm_min, 2); break;
    case 'p': wide_time_text(output, value->tm_hour < 12 ? L"AM" : L"PM"); break;
    case 'S': wide_time_number(output, value->tm_sec, 2); break;
    case 'U':
      week = (value->tm_yday - value->tm_wday + 7) / 7;
      wide_time_number(output, week, 2);
      break;
    case 'w': wide_time_number(output, value->tm_wday, 1); break;
    case 'W':
      week = (value->tm_yday - (value->tm_wday + 6) % 7 + 7) / 7;
      wide_time_number(output, week, 2);
      break;
    case 'x': wide_format_time(output, L"%a %b %d, %Y", value); break;
    case 'X': wide_format_time(output, L"%H:%M:%S", value); break;
    case 'y': wide_time_number(output, value->tm_year % 100, 2); break;
    case 'Y': wide_time_number(output, value->tm_year + 1900, 4); break;
    case 'Z': wide_time_bytes(output, _tzget(value->tm_isdst > 0)); break;
    case '%': wide_time_character(output, '%'); break;
    default: break;
    }
  }
}

size_t wcsftime(wchar_t *destination, size_t maximum,
                const wchar_t *format, const struct tm *value)
{
  wide_time_output output;
  output.destination = destination;
  output.maximum = maximum;
  output.length = 0U;
  wide_format_time(&output, format, value);
  if (output.length + 1U > maximum) {
    if (maximum != 0U)
      destination[0] = 0;
    return 0U;
  }
  destination[output.length] = 0;
  return output.length;
}
