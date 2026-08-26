#include "dc166_stdio.h"
#include <limits.h>

#define WSCAN_H       0x0040U
#define WSCAN_L       0x0080U
#define WSCAN_CAP_L   0x0100U
#define WSCAN_J       0x0200U
#define WSCAN_Z       0x0400U
#define WSCAN_T       0x0800U
#define WSCAN_HH      0x1040U
#define WSCAN_LL      0x1080U

typedef struct {
  FILE *stream;
  int count;
} wide_scanner;

static int wide_get(wide_scanner *scanner)
{
  ++scanner->count;
  return (int)fgetwc(scanner->stream);
}

static void wide_push(wide_scanner *scanner, int character)
{
  ungetwc((wint_t)character, scanner->stream);
  --scanner->count;
}

static int wide_space(int character)
{
  return character == ' ' || character == '\t' || character == '\n' ||
         character == '\r' || character == '\f' || character == '\v';
}

static int wide_digit(int character)
{
  return character >= '0' && character <= '9';
}

static int wide_hex(int character)
{
  if (character >= '0' && character <= '9')
    return character - '0';
  if (character >= 'A' && character <= 'F')
    return character - 'A' + 10;
  if (character >= 'a' && character <= 'f')
    return character - 'a' + 10;
  return -1;
}

static int wide_long_length(unsigned int length)
{
  return length == WSCAN_L || length == WSCAN_LL || length == WSCAN_J;
}

static void wide_store_integer(void *destination, unsigned int length,
                               unsigned long value)
{
  if ((length & WSCAN_L) != 0U)
    *(unsigned long *)destination = value;
  else
    *(unsigned int *)destination = (unsigned int)value;
}

static unsigned long wide_accumulate(unsigned long value, unsigned int base,
                                     unsigned int digit)
{
  if (value > (ULONG_MAX - (unsigned long)digit) / (unsigned long)base)
    return ULONG_MAX;
  return value * (unsigned long)base + (unsigned long)digit;
}

static unsigned long wide_limit(unsigned long magnitude, int negative,
                                int conversion, unsigned int length)
{
  int signed_path = conversion == 'd' || negative;
  if (signed_path) {
    long value;
    if (negative)
      value = magnitude > 0x80000000UL ? LONG_MIN :
          (long)(0UL - magnitude);
    else
      value = magnitude > (unsigned long)LONG_MAX ? LONG_MAX : (long)magnitude;
    if (!wide_long_length(length)) {
      if (length == WSCAN_HH) {
        if (value > (long)SCHAR_MAX)
          value = (long)SCHAR_MAX;
        else if (value < (long)SCHAR_MIN)
          value = (long)SCHAR_MIN;
      } else {
        if (value > (long)INT_MAX)
          value = (long)INT_MAX;
        else if (value < (long)INT_MIN)
          value = (long)INT_MIN;
      }
    }
    return (unsigned long)value;
  }
  if (!wide_long_length(length)) {
    if (length == WSCAN_HH && magnitude > (unsigned long)UCHAR_MAX)
      magnitude = (unsigned long)UCHAR_MAX;
    else if (length != WSCAN_HH && magnitude > (unsigned long)UINT_MAX)
      magnitude = (unsigned long)UINT_MAX;
  }
  return magnitude;
}

static int wide_scan_integer(wide_scanner *scanner, int conversion,
                             unsigned int length, int width,
                             void *destination)
{
  unsigned long value = 0UL;
  unsigned int base;
  int negative = 0;
  int character;
  int digit;
  int start;

  if (width == 0)
    width = -1;
  do {
    character = wide_get(scanner);
  } while (wide_space(character));
  start = scanner->count - 1;
  if (character == '-' || character == '+') {
    negative = character == '-';
    character = wide_get(scanner);
    --width;
  }
  if ((conversion == 'i' || conversion == 'x') && character == '0') {
    if (conversion == 'i')
      conversion = 'o';
    character = wide_get(scanner);
    if (width != 0)
      --width;
    if (character == 'x' || character == 'X') {
      character = wide_get(scanner);
      if (width != 0)
        --width;
      conversion = 'x';
    }
  } else if (conversion == 'i') {
    conversion = 'd';
  }
  base = conversion == 'o' ? 8U : conversion == 'x' ? 16U : 10U;
  while (width != 0) {
    digit = wide_hex(character);
    if (digit < 0 || (unsigned int)digit >= base)
      break;
    value = wide_accumulate(value, base, (unsigned int)digit);
    character = wide_get(scanner);
    --width;
  }
  wide_push(scanner, character);
  if (scanner->count == start)
    return character == (int)WEOF ? WEOF : 0;
  value = wide_limit(value, negative, conversion, length);
  if (destination != 0) {
    wide_store_integer(destination, length, value);
    return 1;
  }
  return 0;
}

static int wide_set_contains(const wchar_t *begin, const wchar_t *end,
                             int character)
{
  while (begin != end) {
    if (*begin++ == (wchar_t)character)
      return 1;
  }
  return 0;
}

static int wide_scan_string(wide_scanner *scanner, int scanset,
                            const wchar_t *begin, const wchar_t *end,
                            int inverted, unsigned int length, int width,
                            void *destination)
{
  int character;
  int matched = 0;
  char *bytes = (char *)destination;
  wchar_t *wide = (wchar_t *)destination;

  if (width == 0)
    width = 30000;
  if (scanset)
    character = wide_get(scanner);
  else {
    do {
      character = wide_get(scanner);
    } while (wide_space(character));
  }
  if (character == (int)WEOF) {
    wide_push(scanner, character);
    return -1;
  }
  while (width-- != 0 && character != (int)WEOF) {
    int accepted = scanset ? wide_set_contains(begin, end, character) :
        !wide_space(character);
    if (scanset && inverted)
      accepted = !accepted;
    if (!accepted) {
      if (!scanset || character != '\n')
        wide_push(scanner, character);
      break;
    }
    if (destination != 0) {
      if ((length & WSCAN_L) != 0U)
        *wide++ = (wchar_t)character;
      else {
        if ((unsigned int)character >= 0x100U) {
          errno = EILSEQ;
          return -1;
        }
        *bytes++ = (char)character;
      }
    }
    matched = 1;
    if (width == 0)
      break;
    character = wide_get(scanner);
  }
  if (destination != 0) {
    if ((length & WSCAN_L) != 0U)
      *wide = 0;
    else
      *bytes = 0;
  }
  return destination != 0 && matched ? 1 : 0;
}

static int wide_prefix(wide_scanner *scanner, const wchar_t *prefix)
{
  while (*prefix != 0) {
    int character = wide_get(scanner);
    if (character != *prefix++) {
      wide_push(scanner, character);
      return 0;
    }
  }
  return 1;
}

static int wide_hex_byte(wide_scanner *scanner, unsigned char *value)
{
  int high = wide_get(scanner);
  int low;
  int high_value = wide_hex(high);
  int low_value;
  if (high_value < 0) {
    wide_push(scanner, high);
    return 0;
  }
  low = wide_get(scanner);
  low_value = wide_hex(low);
  if (low_value < 0) {
    wide_push(scanner, low);
    return 0;
  }
  *value = (unsigned char)((high_value << 4) | low_value);
  return 1;
}

static int wide_scan_pointer(wide_scanner *scanner, unsigned int length,
                             void *destination)
{
  typedef void _near *near_pointer;
  union { void _near *pointer; unsigned char bytes[2]; } near_value;
#if _MODEL != 't'
  typedef void _far *far_pointer;
  typedef void _huge *huge_pointer;
  union { void _far *pointer; unsigned char bytes[4]; } far_value;
  union { void _huge *pointer; unsigned char bytes[4]; } huge_value;
#endif
  int character;

  do { character = wide_get(scanner); } while (wide_space(character));
  wide_push(scanner, character);
#if _MODEL == 't'
  if ((length & (WSCAN_H | WSCAN_L | WSCAN_CAP_L)) == 0U)
    length = WSCAN_H;
#elif _MODEL == 'h'
  if ((length & (WSCAN_H | WSCAN_L | WSCAN_CAP_L)) == 0U)
    length = WSCAN_CAP_L;
#else
  if ((length & (WSCAN_H | WSCAN_L | WSCAN_CAP_L)) == 0U)
    length = WSCAN_L;
#endif
  near_value.pointer = 0;
#if _MODEL == 't'
  (void)length;
  if (!wide_prefix(scanner, L"<near>") ||
      !wide_hex_byte(scanner, &near_value.bytes[1]) ||
      !wide_hex_byte(scanner, &near_value.bytes[0]))
    return -1;
  if (destination != 0)
    *(void **)destination = near_value.pointer;
#else
  far_value.pointer = 0;
  huge_value.pointer = 0;
  if ((length & WSCAN_H) != 0U) {
    if (!wide_prefix(scanner, L"<near>") ||
        !wide_hex_byte(scanner, &near_value.bytes[1]) ||
        !wide_hex_byte(scanner, &near_value.bytes[0]))
      return -1;
    if (destination != 0)
      *(near_pointer *)destination = near_value.pointer;
  } else if ((length & WSCAN_L) != 0U) {
    if (!wide_prefix(scanner, L"<far>") ||
        !wide_hex_byte(scanner, &far_value.bytes[3]) ||
        !wide_hex_byte(scanner, &far_value.bytes[2]))
      return -1;
    character = wide_get(scanner);
    if (character != ':') {
      wide_push(scanner, character);
      return -1;
    }
    if (!wide_hex_byte(scanner, &far_value.bytes[1]) ||
        !wide_hex_byte(scanner, &far_value.bytes[0]))
      return -1;
    if (destination != 0)
      *(far_pointer *)destination = far_value.pointer;
  } else {
    if (!wide_prefix(scanner, L"<huge>") ||
        !wide_hex_byte(scanner, &huge_value.bytes[2]))
      return -1;
    character = wide_get(scanner);
    if (character != ':') {
      wide_push(scanner, character);
      return -1;
    }
    if (!wide_hex_byte(scanner, &huge_value.bytes[1]) ||
        !wide_hex_byte(scanner, &huge_value.bytes[0]))
      return -1;
    if (destination != 0)
      *(huge_pointer *)destination = huge_value.pointer;
  }
#endif
  character = wide_get(scanner);
  wide_push(scanner, character);
  return destination != 0 ? 1 : 0;
}

static unsigned int wide_length(const wchar_t **format)
{
  unsigned int length = 0U;
  if (**format == 'l') {
    ++*format;
    if (**format == 'l') { ++*format; length = WSCAN_LL; }
    else length = WSCAN_L;
  } else if (**format == 'h') {
    ++*format;
    if (**format == 'h') { ++*format; length = WSCAN_HH; }
    else length = WSCAN_H;
  } else if (**format == 'j') { ++*format; length = WSCAN_J; }
  else if (**format == 'z') { ++*format; length = WSCAN_Z; }
  else if (**format == 't') { ++*format; length = WSCAN_T; }
  else if (**format == 'L') { ++*format; length = WSCAN_CAP_L; }
  return length;
}

static int wide_failure(int pending, int assignments)
{
  return pending ? WEOF : assignments;
}

int _dowscan(FILE *stream, const wchar_t *format, va_list arguments)
{
  wide_scanner scanner;
  int assignments = 0;
  int pending = 1;
  scanner.stream = stream;
  scanner.count = 0;

  while (*format != 0) {
    int format_character = *format++;
    if (format_character == ' ' || format_character == '\t') {
      int character;
      do { character = wide_get(&scanner); } while (wide_space(character));
      wide_push(&scanner, character);
      continue;
    }
    if (format_character != '%' || *format == '%') {
      int character;
      if (format_character == '%')
        ++format;
      character = wide_get(&scanner);
      if (character != format_character) {
        wide_push(&scanner, character);
        return assignments;
      }
      continue;
    }
    {
      int suppress = 0;
      int width = 0;
      unsigned int length;
      int conversion;
      void *destination = 0;
      if (*format == '*') { suppress = 1; ++format; }
      while (wide_digit(*format))
        width = width * 10 + *format++ - '0';
      length = wide_length(&format);
      conversion = *format++;
      if (conversion == 'X')
        conversion = 'x';

      if (conversion == 'd' || conversion == 'i' || conversion == 'o' ||
          conversion == 'u' || conversion == 'x') {
        int result;
        if (!suppress)
          destination = va_arg(arguments, void *);
        result = wide_scan_integer(&scanner, conversion, length, width,
                                   destination);
        if (result == (int)WEOF)
          return wide_failure(pending, assignments);
        pending = 0;
        assignments += result;
        continue;
      }
      if (conversion == 'c') {
        int remaining = width == 0 ? 1 : width;
        int matched = 0;
        char *bytes = 0;
        wchar_t *wide = 0;
        if (!suppress) {
          destination = va_arg(arguments, void *);
          bytes = (char *)destination;
          wide = (wchar_t *)destination;
        }
        while (remaining-- != 0) {
          int character = wide_get(&scanner);
          if (character == (int)WEOF) {
            if (!suppress && matched != 0) { ++assignments; pending = 0; }
            return wide_failure(pending, assignments);
          }
          if (!suppress) {
            if ((length & WSCAN_L) != 0U)
              wide[matched] = (wchar_t)character;
            else
              bytes[matched] = (char)character;
          }
          ++matched;
        }
        if (!suppress) {
          int character = wide_get(&scanner);
          wide_push(&scanner, character);
          ++assignments;
          pending = 0;
        }
        continue;
      }
      if (conversion == '[' || conversion == 's') {
        const wchar_t *begin = 0;
        const wchar_t *end = 0;
        int inverted = 0;
        int result;
        if (conversion == '[') {
          begin = format;
          if (*begin == '^') { inverted = 1; ++begin; }
          end = begin;
          if (*end == ']') ++end;
          while (*end != 0 && *end != ']') ++end;
          format = *end == ']' ? end + 1 : end;
        }
        if (!suppress)
          destination = va_arg(arguments, void *);
        result = wide_scan_string(&scanner, conversion == '[', begin, end,
                                  inverted, length, width, destination);
        if (result == (int)WEOF)
          return wide_failure(pending, assignments);
        if (result == 0 && conversion == '[' && !suppress)
          return assignments;
        assignments += result;
        pending = 0;
        continue;
      }
      if (conversion == 'p') {
        int result;
        if (!suppress)
          destination = va_arg(arguments, void *);
        result = wide_scan_pointer(&scanner, length, destination);
        if (result == (int)WEOF)
          return wide_failure(pending, assignments);
        assignments += result;
        pending = 0;
        continue;
      }
      if (conversion == 'n') {
        if (!suppress) {
          destination = va_arg(arguments, void *);
          wide_store_integer(destination, length,
                             (unsigned long)scanner.count);
        }
        continue;
      }
      if (conversion == 'E' || conversion == 'G' || conversion == 'e' ||
          conversion == 'f' || conversion == 'g') {
        errno = ERR_NOFLOAT;
        return assignments;
      }
      errno = ERR_FORMAT;
      return assignments;
    }
  }
  return assignments;
}
