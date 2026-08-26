typedef unsigned int size_t;
typedef unsigned long time_t;
typedef unsigned long clock_t;

#define TIME_ERROR 0xffffffffUL
#define CLOCKS_PER_SECOND 1000UL
#define SECONDS_PER_DAY 86400UL
#define EPOCH_1900_TO_1970 2208988800UL

struct tm {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

extern clock_t clock(void);

const int _cumdpm[12] = {
  0, 31, 59, 90, 120, 151,
  181, 212, 243, 273, 304, 334
};

static const char short_weekdays[7][4] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *const long_weekdays[7] = {
  "Sunday", "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday"
};

static const char short_months[12][4] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char *const long_months[12] = {
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};

static struct tm broken_time;
static char asctime_buffer[26];
static time_t epoch_at_clock_zero = TIME_ERROR;

/* The Small library stores three fixed six-byte fields. */
static char timezone_fields[3][6] = {
  { 'U', 'T', 'C', 0, 0, 0 },
  { 'U', 'T', 'C', 0, 0, 0 },
  { '+', '0', '0', '0', '0', 0 }
};

static int leap_tm_year(int tm_year)
{
  return (tm_year & 3) == 0;
}

static long timezone_minutes(void)
{
  const char *text = timezone_fields[2];
  long value = 0L;
  int negative = 0;

  if (*text == '-' || *text == '+') {
    negative = *text == '-';
    ++text;
  }
  while (*text >= '0' && *text <= '9') {
    value = value * 10L + (long)(*text - '0');
    ++text;
  }
  if (negative)
    value = -value;
  if (value <= -780L || value >= 780L)
    return 0L;
  return value;
}

char *_tzget(int field)
{
  if (field < 0 || field > 2)
    return 0;
  return timezone_fields[field];
}

int _tzset(const char *text)
{
  char replacement[3][6];
  int index;
  int hours;
  int digit;

  if (text == 0)
    return 0;
  for (index = 0; index != 3; ++index) {
    char character = *text++;
    if (!((character >= 'A' && character <= 'Z') ||
          (character >= 'a' && character <= 'z')))
      return 0;
    replacement[0][index] = character;
  }
  replacement[0][3] = 0;
  replacement[0][4] = 0;
  replacement[0][5] = 0;

  if (*text == '-' || *text == '+')
    replacement[2][0] = *text++;
  else
    replacement[2][0] = '+';

  if (*text < '0' || *text > '9')
    return 0;
  hours = *text++ - '0';
  if (*text >= '0' && *text <= '9')
    hours = hours * 10 + (*text++ - '0');

  hours *= 60;
  for (index = 4; index != 0; --index) {
    digit = hours % 10;
    hours /= 10;
    replacement[2][index] = (char)('0' + digit);
  }
  replacement[2][5] = 0;

  for (index = 0; index != 3; ++index) {
    char character = *text++;
    if (!((character >= 'A' && character <= 'Z') ||
          (character >= 'a' && character <= 'z')))
      return 0;
    replacement[1][index] = character;
  }
  if (*text != 0)
    return 0;
  replacement[1][3] = 0;
  replacement[1][4] = 0;
  replacement[1][5] = 0;

  for (index = 0; index != 18; ++index)
    ((char *)timezone_fields)[index] = ((char *)replacement)[index];
  return 1;
}

struct tm *gmtime(const time_t *timer)
{
  unsigned long days = *timer / SECONDS_PER_DAY;
  unsigned long seconds = *timer % SECONDS_PER_DAY;
  unsigned long remaining;
  int tm_year = 70;
  int month;
  int days_in_year;
  int days_in_month;

  broken_time.tm_hour = (int)(seconds / 3600UL);
  seconds %= 3600UL;
  broken_time.tm_min = (int)(seconds / 60UL);
  broken_time.tm_sec = (int)(seconds % 60UL);
  broken_time.tm_wday = (int)((days + 4UL) % 7UL);

  remaining = days;
  for (;;) {
    days_in_year = leap_tm_year(tm_year) ? 366 : 365;
    if (remaining < (unsigned long)days_in_year)
      break;
    remaining -= (unsigned long)days_in_year;
    ++tm_year;
  }

  broken_time.tm_year = tm_year;
  broken_time.tm_yday = (int)remaining;
  month = 0;
  for (;;) {
    days_in_month = month == 1 && leap_tm_year(tm_year) ? 29 :
        (_cumdpm[(month + 1) % 12] - _cumdpm[month]);
    if (month == 11)
      days_in_month = 31;
    if (remaining < (unsigned long)days_in_month)
      break;
    remaining -= (unsigned long)days_in_month;
    ++month;
  }
  broken_time.tm_mon = month;
  broken_time.tm_mday = (int)remaining + 1;
  broken_time.tm_isdst = -1;
  return &broken_time;
}

struct tm *localtime(const time_t *timer)
{
  time_t adjusted = *timer + (time_t)(timezone_minutes() * 60L);
  struct tm *result = gmtime(&adjusted);
  result->tm_isdst = -1;
  return result;
}

time_t mktime(struct tm *value)
{
  long month = value->tm_mon;
  long tm_year = value->tm_year + month / 12L;
  long days;
  long yday;
  time_t result;

  month %= 12L;
  if (month < 0L) {
    month += 12L;
    --tm_year;
  }
  if (tm_year < 70L || tm_year > 205L)
    return TIME_ERROR;

  yday = (long)_cumdpm[(int)month] +
      (long)value->tm_mday - 1L;
  if (leap_tm_year((int)tm_year) && month > 1L)
    ++yday;
  days = tm_year * 365L + (tm_year - 1L) / 4L + yday;

  value->tm_wday = (int)((days + 1L) % 7L);
  value->tm_yday = (int)yday;

  result = (time_t)days * SECONDS_PER_DAY;
  result += (time_t)((long)value->tm_hour * 3600L);
  result += (time_t)((long)value->tm_min * 60L);
  result += (time_t)value->tm_sec;
  result -= EPOCH_1900_TO_1970;
  result -= (time_t)(timezone_minutes() * 60L);
  return result;
}

time_t time(time_t *destination)
{
  time_t current;
  current = epoch_at_clock_zero;
  if (current != TIME_ERROR)
    current += clock() / CLOCKS_PER_SECOND;
  if (destination != 0)
    *destination = current;
  return current;
}

void _stime(time_t *source)
{
  epoch_at_clock_zero = *source - clock() / CLOCKS_PER_SECOND;
}

typedef struct output_buffer {
  char *destination;
  size_t maximum;
  size_t length;
} output_buffer;

static void append_character(output_buffer *output, char character)
{
  if (output->length < output->maximum)
    output->destination[output->length] = character;
  ++output->length;
}

static void append_text(output_buffer *output, const char *text)
{
  while (*text != 0)
    append_character(output, *text++);
}

static void append_number(output_buffer *output, int value, int digits)
{
  char buffer[6];
  int index = digits;

  while (index != 0) {
    buffer[--index] = (char)('0' + value % 10);
    value /= 10;
  }
  for (index = 0; index != digits; ++index)
    append_character(output, buffer[index]);
}

static void format_time(output_buffer *output, const char *format,
                        const struct tm *value)
{
  while (*format != 0) {
    char specifier;
    int hour;
    int week;

    if (*format != '%') {
      append_character(output, *format++);
      continue;
    }
    ++format;
    if (*format == 0) {
      append_character(output, '%');
      break;
    }
    specifier = *format++;
    switch (specifier) {
    case 'a': append_text(output, short_weekdays[value->tm_wday]); break;
    case 'A': append_text(output, long_weekdays[value->tm_wday]); break;
    case 'b': append_text(output, short_months[value->tm_mon]); break;
    case 'B': append_text(output, long_months[value->tm_mon]); break;
    case 'c': format_time(output, "%a %b %d %H:%M:%S %Y", value); break;
    case 'd': append_number(output, value->tm_mday, 2); break;
    case 'H': append_number(output, value->tm_hour, 2); break;
    case 'I':
      hour = value->tm_hour < 13 ? value->tm_hour : value->tm_hour - 12;
      append_number(output, hour == 0 ? 12 : hour, 2);
      break;
    case 'j': append_number(output, value->tm_yday + 1, 3); break;
    case 'm': append_number(output, value->tm_mon + 1, 2); break;
    case 'M': append_number(output, value->tm_min, 2); break;
    case 'p': append_text(output, value->tm_hour < 12 ? "AM" : "PM"); break;
    case 'S': append_number(output, value->tm_sec, 2); break;
    case 'U':
      week = (value->tm_yday - value->tm_wday + 7) / 7;
      append_number(output, week, 2);
      break;
    case 'w': append_number(output, value->tm_wday, 1); break;
    case 'W':
      week = (value->tm_yday - (value->tm_wday + 6) % 7 + 7) / 7;
      append_number(output, week, 2);
      break;
    case 'x': format_time(output, "%a %b %d, %Y", value); break;
    case 'X': format_time(output, "%H:%M:%S", value); break;
    case 'y': append_number(output, value->tm_year % 100, 2); break;
    case 'Y': append_number(output, value->tm_year + 1900, 4); break;
    case 'Z': append_text(output, _tzget(value->tm_isdst > 0)); break;
    case '%': append_character(output, '%'); break;
    default: append_character(output, specifier); break;
    }
  }
}

size_t strftime(char *destination, size_t maximum, const char *format,
                const struct tm *value)
{
  output_buffer output;

  output.destination = destination;
  output.maximum = maximum;
  output.length = 0U;
  format_time(&output, format, value);
  if (output.length + 1U > maximum)
    return 0U;
  destination[output.length] = 0;
  return output.length;
}

char *asctime(const struct tm *value)
{
  (void)strftime(asctime_buffer, sizeof(asctime_buffer), "%c\n", value);
  return asctime_buffer;
}

char *ctime(const time_t *timer)
{
  return asctime(localtime(timer));
}
