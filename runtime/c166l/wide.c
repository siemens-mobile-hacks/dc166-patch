typedef unsigned int size_t;
typedef int wchar_t;

#define ERANGE 24
#define LONG_MAX_VALUE 0x7fffffffUL
#define LONG_MIN_MAGNITUDE 0x80000000UL
#define ULONG_MAX_VALUE 0xffffffffUL

extern int errno;
extern int towupper(int);

size_t wcslen(const wchar_t *source)
{
  const wchar_t *end = source;
  while (*end != 0)
    ++end;
  return (size_t)(end - source);
}

wchar_t *wcscpy(wchar_t *destination, const wchar_t *source)
{
  wchar_t *result = destination;
  while ((*destination++ = *source++) != 0)
    ;
  return result;
}

wchar_t *wcsncpy(wchar_t *destination, const wchar_t *source, size_t count)
{
  wchar_t *result = destination;
  while (count != 0U && *source != 0) {
    *destination++ = *source++;
    --count;
  }
  while (count-- != 0U)
    *destination++ = 0;
  return result;
}

wchar_t *wcscat(wchar_t *destination, const wchar_t *source)
{
  wchar_t *result = destination;
  while (*destination != 0)
    ++destination;
  while ((*destination++ = *source++) != 0)
    ;
  return result;
}

wchar_t *wcsncat(wchar_t *destination, const wchar_t *source, size_t count)
{
  wchar_t *result = destination;
  while (*destination != 0)
    ++destination;
  while (count != 0U && *source != 0) {
    *destination++ = *source++;
    --count;
  }
  *destination = 0;
  return result;
}

int wcscmp(const wchar_t *left, const wchar_t *right)
{
  while (*left != 0 && *left == *right) {
    ++left;
    ++right;
  }
  return *left - *right;
}

int wcsncmp(const wchar_t *left, const wchar_t *right, size_t count)
{
  while (count != 0U) {
    if (*left != *right)
      return *left - *right;
    if (*left == 0)
      return 0;
    ++left;
    ++right;
    --count;
  }
  return 0;
}

int _wcscasecmp(const wchar_t *left, const wchar_t *right)
{
  int left_value;
  int right_value;

  do {
    left_value = towupper(*left++);
    right_value = towupper(*right++);
  } while (left_value != 0 && left_value == right_value);
  return left_value - right_value;
}

int _wcsncasecmp(const wchar_t *left, const wchar_t *right, size_t count)
{
  int left_value;
  int right_value;

  while (count-- != 0U) {
    left_value = towupper(*left++);
    right_value = towupper(*right++);
    if (left_value == 0 || left_value != right_value)
      return left_value - right_value;
  }
  return 0;
}

int wcscoll(const wchar_t *left, const wchar_t *right)
{
  return wcscmp(left, right);
}

size_t wcsxfrm(wchar_t *destination, const wchar_t *source, size_t count)
{
  size_t length = wcslen(source);
  size_t index;
  size_t copied = length < count ? length + 1U : count;
  for (index = 0U; index < copied; ++index)
    destination[index] = source[index];
  return length;
}

wchar_t *wcschr(const wchar_t *source, wchar_t character)
{
  do {
    if (*source == character)
      return (wchar_t *)source;
  } while (*source++ != 0);
  return 0;
}

wchar_t *wcsrchr(const wchar_t *source, wchar_t character)
{
  const wchar_t *result = 0;
  do {
    if (*source == character)
      result = source;
  } while (*source++ != 0);
  return (wchar_t *)result;
}

size_t wcsspn(const wchar_t *source, const wchar_t *accept)
{
  const wchar_t *start = source;
  while (*source != 0) {
    const wchar_t *candidate = accept;
    while (*candidate != 0 && *candidate != *source)
      ++candidate;
    if (*candidate == 0)
      break;
    ++source;
  }
  return (size_t)(source - start);
}

size_t wcscspn(const wchar_t *source, const wchar_t *reject)
{
  const wchar_t *start = source;
  while (*source != 0) {
    const wchar_t *candidate = reject;
    while (*candidate != 0) {
      if (*candidate++ == *source)
        return (size_t)(source - start);
    }
    ++source;
  }
  return (size_t)(source - start);
}

wchar_t *wcspbrk(const wchar_t *source, const wchar_t *accept)
{
  source += wcscspn(source, accept);
  return *source == 0 ? 0 : (wchar_t *)source;
}

wchar_t *wcsstr(const wchar_t *source, const wchar_t *needle)
{
  size_t needle_length = wcslen(needle);
  if (needle_length == 0U)
    return (wchar_t *)source;
  while (*source != 0) {
    if (*source == *needle && wcsncmp(source, needle, needle_length) == 0)
      return (wchar_t *)source;
    ++source;
  }
  return 0;
}

wchar_t *wcstok(wchar_t *source, const wchar_t *delimiters,
                 wchar_t **context)
{
  wchar_t *token;
  wchar_t *separator;
  if (source == 0)
    source = *context;
  if (source == 0)
    return 0;
  source += wcsspn(source, delimiters);
  if (*source == 0) {
    *context = 0;
    return 0;
  }
  token = source;
  separator = wcspbrk(source, delimiters);
  if (separator == 0) {
    *context = 0;
  } else {
    *separator = 0;
    *context = separator + 1;
  }
  return token;
}

wchar_t *wmemchr(const wchar_t *source, wchar_t character, size_t count)
{
  while (count-- != 0U) {
    if (*source == character)
      return (wchar_t *)source;
    ++source;
  }
  return 0;
}

int wmemcmp(const wchar_t *left, const wchar_t *right, size_t count)
{
  while (count-- != 0U) {
    if (*left != *right)
      return *left - *right;
    ++left;
    ++right;
  }
  return 0;
}

wchar_t *wmemcpy(wchar_t *destination, const wchar_t *source, size_t count)
{
  wchar_t *result = destination;
  while (count-- != 0U)
    *destination++ = *source++;
  return result;
}

wchar_t *wmemmove(wchar_t *destination, const wchar_t *source, size_t count)
{
  wchar_t *result = destination;
  if (destination < source) {
    while (count-- != 0U)
      *destination++ = *source++;
  } else if (destination != source) {
    destination += count;
    source += count;
    while (count-- != 0U)
      *--destination = *--source;
  }
  return result;
}

wchar_t *wmemset(wchar_t *destination, wchar_t character, size_t count)
{
  wchar_t *result = destination;
  while (count-- != 0U)
    *destination++ = character;
  return result;
}

static int wide_space(wchar_t character)
{
  return character == ' ' ||
      (character >= '\t' && character <= '\r');
}

static int wide_digit(wchar_t character)
{
  if (character >= '0' && character <= '9')
    return character - '0';
  if (character >= 'a' && character <= 'z')
    return character - 'a' + 10;
  if (character >= 'A' && character <= 'Z')
    return character - 'A' + 10;
  return -1;
}

static unsigned long parse_wide(const wchar_t *text, wchar_t **end,
                                int base, int *negative, int *overflow)
{
  const wchar_t *origin = text;
  unsigned long value = 0UL;
  int digits = 0;
  int digit;
  while (wide_space(*text))
    ++text;
  *negative = 0;
  *overflow = 0;
  if (*text == '+' || *text == '-') {
    *negative = *text == '-';
    ++text;
  }
  if (base != 0 && (base < 2 || base > 36)) {
    errno = 13;
    if (end != 0)
      *end = (wchar_t *)origin;
    return 0UL;
  }
  if ((base == 0 || base == 16) && text[0] == '0' &&
      (text[1] == 'x' || text[1] == 'X') &&
      wide_digit(text[2]) >= 0 && wide_digit(text[2]) < 16) {
    base = 16;
    text += 2;
  } else if (base == 0) {
    base = *text == '0' ? 8 : 10;
  }
  while ((digit = wide_digit(*text)) >= 0 && digit < base) {
    ++digits;
    if (value > (ULONG_MAX_VALUE - (unsigned long)digit) /
                (unsigned long)base)
      *overflow = 1;
    else if (!*overflow)
      value = value * (unsigned long)base + (unsigned long)digit;
    ++text;
  }
  if (end != 0)
    *end = digits != 0 ? (wchar_t *)text : (wchar_t *)origin;
  return value;
}

long wcstol(const wchar_t *text, wchar_t **end, int base)
{
  unsigned long value;
  unsigned long limit;
  int negative;
  int overflow;
  value = parse_wide(text, end, base, &negative, &overflow);
  limit = negative ? LONG_MIN_MAGNITUDE : LONG_MAX_VALUE;
  if (!overflow && value > limit)
    overflow = 1;
  if (overflow) {
    errno = ERANGE;
    return negative ? (long)LONG_MIN_MAGNITUDE : (long)LONG_MAX_VALUE;
  }
  return negative ? (long)(0UL - value) : (long)value;
}

unsigned long wcstoul(const wchar_t *text, wchar_t **end, int base)
{
  unsigned long value;
  int negative;
  int overflow;
  value = parse_wide(text, end, base, &negative, &overflow);
  if (overflow) {
    errno = ERANGE;
    return ULONG_MAX_VALUE;
  }
  return negative ? 0UL - value : value;
}
