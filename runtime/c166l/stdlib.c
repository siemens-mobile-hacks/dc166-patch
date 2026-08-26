typedef unsigned int size_t;

#define EINVAL 13
#define ERANGE 24
#define LONG_MAX_VALUE 0x7fffffffUL
#define LONG_MIN_MAGNITUDE 0x80000000UL
#define ULONG_MAX_VALUE 0xffffffffUL

int errno;

static int ascii_space(unsigned char character)
{
  return character == ' ' ||
      (character >= '\t' && character <= '\r');
}

static int digit_value(unsigned char character)
{
  if (character >= '0' && character <= '9')
    return character - '0';
  if (character >= 'a' && character <= 'z')
    return character - 'a' + 10;
  if (character >= 'A' && character <= 'Z')
    return character - 'A' + 10;
  return -1;
}

static unsigned long parse_unsigned(const char *text, char **end,
                                    int base, unsigned long limit,
                                    int *negative, int *overflow)
{
  const char *origin = text;
  unsigned long value = 0UL;
  int digits = 0;
  int digit;

  while (ascii_space((unsigned char)*text))
    ++text;
  *negative = 0;
  *overflow = 0;
  if (*text == '+' || *text == '-') {
    *negative = *text == '-';
    ++text;
  }

  if (base != 0 && (base < 2 || base > 36)) {
    errno = EINVAL;
    if (end != 0)
      *end = (char *)origin;
    return 0UL;
  }

  if ((base == 0 || base == 16) && text[0] == '0' &&
      (text[1] == 'x' || text[1] == 'X') &&
      digit_value((unsigned char)text[2]) >= 0 &&
      digit_value((unsigned char)text[2]) < 16) {
    base = 16;
    text += 2;
  } else if (base == 0) {
    base = *text == '0' ? 8 : 10;
  }

  while ((digit = digit_value((unsigned char)*text)) >= 0 && digit < base) {
    ++digits;
    if (value > (limit - (unsigned long)digit) / (unsigned long)base) {
      *overflow = 1;
    } else if (!*overflow) {
      value = value * (unsigned long)base + (unsigned long)digit;
    }
    ++text;
  }

  if (end != 0)
    *end = digits != 0 ? (char *)text : (char *)origin;
  return value;
}

long strtol(const char *text, char **end, int base)
{
  unsigned long value;
  unsigned long limit;
  int negative;
  int overflow;

  limit = LONG_MAX_VALUE;
  value = parse_unsigned(text, end, base, ULONG_MAX_VALUE,
                         &negative, &overflow);
  if (negative)
    limit = LONG_MIN_MAGNITUDE;
  if (!overflow && value > limit)
    overflow = 1;
  if (overflow) {
    errno = ERANGE;
    return negative ? (long)LONG_MIN_MAGNITUDE : (long)LONG_MAX_VALUE;
  }
  if (negative)
    return (long)(0UL - value);
  return (long)value;
}

unsigned long strtoul(const char *text, char **end, int base)
{
  unsigned long value;
  int negative;
  int overflow;

  value = parse_unsigned(text, end, base, ULONG_MAX_VALUE,
                         &negative, &overflow);
  if (overflow) {
    errno = ERANGE;
    return ULONG_MAX_VALUE;
  }
  return negative ? 0UL - value : value;
}

int atoi(const char *text)
{
  return (int)strtol(text, 0, 10);
}

long atol(const char *text)
{
  return strtol(text, 0, 10);
}
