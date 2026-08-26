#include "dc166_stdio.h"
#include <limits.h>

#define DC166_SCAN_H       0x0040U
#define DC166_SCAN_L       0x0080U
#define DC166_SCAN_CAP_L   0x0100U
#define DC166_SCAN_J       0x0200U
#define DC166_SCAN_Z       0x0400U
#define DC166_SCAN_T       0x0800U
#define DC166_SCAN_HH      0x1040U
#define DC166_SCAN_LL      0x1080U
#define DC166_SCAN_SET     0x2000U

typedef struct {
  FILE *stream;
  int count;
} dc166_scanner;

static int scan_get(dc166_scanner *scanner)
{
  ++scanner->count;
  return fgetc(scanner->stream);
}

static void scan_push(dc166_scanner *scanner, int character)
{
  ungetc(character, scanner->stream);
  --scanner->count;
}

static int scan_space(int character)
{
  return character == ' ' || character == '\t' || character == '\n' ||
         character == '\r' || character == '\f' || character == '\v';
}

static int scan_digit(int character)
{
  return character >= '0' && character <= '9';
}

static int scan_hex_value(int character)
{
  if (character >= '0' && character <= '9')
    return character - '0';
  if (character >= 'A' && character <= 'F')
    return character - 'A' + 10;
  if (character >= 'a' && character <= 'f')
    return character - 'a' + 10;
  return -1;
}

static int scan_is_long(unsigned int length)
{
  return length == DC166_SCAN_L || length == DC166_SCAN_LL ||
         length == DC166_SCAN_J;
}

static void scan_store(void *destination, unsigned int length,
                       unsigned long value)
{
  if ((length & DC166_SCAN_L) != 0U)
    *(unsigned long *)destination = value;
  else
    *(unsigned int *)destination = (unsigned int)value;
}

static unsigned long scan_accumulate(unsigned long value,
                                     unsigned int base,
                                     unsigned int digit)
{
  if (value > (ULONG_MAX - (unsigned long)digit) / (unsigned long)base)
    return ULONG_MAX;
  return value * (unsigned long)base + (unsigned long)digit;
}

static unsigned long scan_limit_integer(unsigned long magnitude,
                                        int negative, int conversion,
                                        unsigned int length)
{
  int signed_path = conversion == 'd' || negative;

  if (signed_path) {
    long value;

    if (negative) {
      if (magnitude > 0x80000000UL)
        value = LONG_MIN;
      else
        value = (long)(0UL - magnitude);
    } else if (magnitude > (unsigned long)LONG_MAX) {
      value = LONG_MAX;
    } else {
      value = (long)magnitude;
    }

    if (!scan_is_long(length)) {
      if (length == DC166_SCAN_HH) {
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

  if (!scan_is_long(length)) {
    if (length == DC166_SCAN_HH && magnitude > (unsigned long)UCHAR_MAX)
      magnitude = (unsigned long)UCHAR_MAX;
    else if (length != DC166_SCAN_HH &&
             magnitude > (unsigned long)UINT_MAX)
      magnitude = (unsigned long)UINT_MAX;
  }
  return magnitude;
}

static int scan_integer(dc166_scanner *scanner, int conversion,
                        unsigned int length, int width, void *destination)
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
    character = scan_get(scanner);
  } while (scan_space(character));
  start = scanner->count - 1;

  if (character == '-' || character == '+') {
    negative = character == '-';
    character = scan_get(scanner);
    --width;
  }

  if ((conversion == 'i' || conversion == 'x') && character == '0') {
    if (conversion == 'i')
      conversion = 'o';
    character = scan_get(scanner);
    if (width != 0)
      --width;
    if (character == 'x' || character == 'X') {
      character = scan_get(scanner);
      if (width != 0)
        --width;
      conversion = 'x';
    }
  } else if (conversion == 'i') {
    conversion = 'd';
  }

  if (conversion == 'd' || conversion == 'u')
    base = 10U;
  else if (conversion == 'o')
    base = 8U;
  else
    base = 16U;

  while (width != 0) {
    digit = scan_hex_value(character);
    if (digit < 0 || (unsigned int)digit >= base)
      break;
    value = scan_accumulate(value, base, (unsigned int)digit);
    character = scan_get(scanner);
    --width;
  }
  scan_push(scanner, character);

  if (scanner->count == start)
    return character == EOF ? EOF : 0;
  value = scan_limit_integer(value, negative, conversion, length);
  if (destination != 0) {
    scan_store(destination, length, value);
    return 1;
  }
  return 0;
}

static int scan_set_contains(const char *begin, const char *end,
                             int character)
{
  while (begin != end) {
    if ((unsigned char)*begin == (unsigned int)character)
      return 1;
    ++begin;
  }
  return 0;
}

static int scan_string(dc166_scanner *scanner, int scanset,
                       const char *set_begin, const char *set_end,
                       int inverted, int width, char *destination)
{
  int character;
  int matched = 0;

  if (width == 0)
    width = 30000;
  if (scanset) {
    character = scan_get(scanner);
  } else {
    do {
      character = scan_get(scanner);
    } while (scan_space(character));
  }

  if (character == EOF) {
    scan_push(scanner, character);
    return EOF;
  }

  while (width-- != 0 && character != EOF) {
    int accepted;

    if (scanset) {
      accepted = scan_set_contains(set_begin, set_end, character);
      if (inverted)
        accepted = !accepted;
    } else {
      accepted = !scan_space(character);
    }
    if (!accepted) {
      if (!scanset || character != '\n')
        scan_push(scanner, character);
      break;
    }
    if (destination != 0)
      *destination++ = (char)character;
    matched = 1;
    if (width == 0)
      break;
    character = scan_get(scanner);
  }

  if (destination != 0)
    *destination = 0;
  return destination != 0 && matched ? 1 : 0;
}

static int scan_hex_byte(dc166_scanner *scanner, unsigned char *value)
{
  int high = scan_get(scanner);
  int low;
  int high_value = scan_hex_value(high);
  int low_value;

  if (high_value < 0) {
    scan_push(scanner, high);
    return 0;
  }
  low = scan_get(scanner);
  low_value = scan_hex_value(low);
  if (low_value < 0) {
    scan_push(scanner, low);
    return 0;
  }
  *value = (unsigned char)((high_value << 4) | low_value);
  return 1;
}

static int scan_prefix(dc166_scanner *scanner, const char *prefix)
{
  int character;

  while (*prefix != 0) {
    character = scan_get(scanner);
    if (character != (unsigned char)*prefix++) {
      scan_push(scanner, character);
      return 0;
    }
  }
  return 1;
}

static int scan_pointer(dc166_scanner *scanner, unsigned int length,
                        void *destination)
{
  typedef void _near *near_pointer;
  union {
    void _near *pointer;
    unsigned char bytes[2];
  } near_value;
#if _MODEL != 't'
  typedef void _far *far_pointer;
  typedef void _huge *huge_pointer;
  union {
    void _far *pointer;
    unsigned char bytes[4];
  } far_value;
  union {
    void _huge *pointer;
    unsigned char bytes[4];
  } huge_value;
#endif
  int character;

  do {
    character = scan_get(scanner);
  } while (scan_space(character));
  scan_push(scanner, character);

#if _MODEL == 't'
  if ((length & (DC166_SCAN_H | DC166_SCAN_L | DC166_SCAN_CAP_L)) == 0U)
    length = DC166_SCAN_H;
#elif _MODEL == 'h'
  if ((length & (DC166_SCAN_H | DC166_SCAN_L | DC166_SCAN_CAP_L)) == 0U)
    length = DC166_SCAN_CAP_L;
#else
  if ((length & (DC166_SCAN_H | DC166_SCAN_L | DC166_SCAN_CAP_L)) == 0U)
    length = DC166_SCAN_L;
#endif

  near_value.pointer = 0;
#if _MODEL == 't'
  (void)length;
  if (!scan_prefix(scanner, "<near>") ||
      !scan_hex_byte(scanner, &near_value.bytes[1]) ||
      !scan_hex_byte(scanner, &near_value.bytes[0]))
    return EOF;
  if (destination != 0)
    *(void **)destination = near_value.pointer;
#else
  far_value.pointer = 0;
  huge_value.pointer = 0;
  if ((length & DC166_SCAN_H) != 0U) {
    if (!scan_prefix(scanner, "<near>") ||
        !scan_hex_byte(scanner, &near_value.bytes[1]) ||
        !scan_hex_byte(scanner, &near_value.bytes[0]))
      return EOF;
    if (destination != 0)
      *(near_pointer *)destination = near_value.pointer;
  } else if ((length & DC166_SCAN_L) != 0U) {
    if (!scan_prefix(scanner, "<far>") ||
        !scan_hex_byte(scanner, &far_value.bytes[3]) ||
        !scan_hex_byte(scanner, &far_value.bytes[2]))
      return EOF;
    character = scan_get(scanner);
    if (character != ':') {
      scan_push(scanner, character);
      return EOF;
    }
    if (!scan_hex_byte(scanner, &far_value.bytes[1]) ||
        !scan_hex_byte(scanner, &far_value.bytes[0]))
      return EOF;
    if (destination != 0)
      *(far_pointer *)destination = far_value.pointer;
  } else {
    if (!scan_prefix(scanner, "<huge>") ||
        !scan_hex_byte(scanner, &huge_value.bytes[2]))
      return EOF;
    character = scan_get(scanner);
    if (character != ':') {
      scan_push(scanner, character);
      return EOF;
    }
    if (!scan_hex_byte(scanner, &huge_value.bytes[1]) ||
        !scan_hex_byte(scanner, &huge_value.bytes[0]))
      return EOF;
    if (destination != 0)
      *(huge_pointer *)destination = huge_value.pointer;
  }
#endif

  character = scan_get(scanner);
  scan_push(scanner, character);
  return destination != 0 ? 1 : 0;
}

static unsigned int scan_length(const char **format)
{
  unsigned int length = 0U;

  if (**format == 'l') {
    ++*format;
    if (**format == 'l') {
      ++*format;
      length = DC166_SCAN_LL;
    } else {
      length = DC166_SCAN_L;
    }
  } else if (**format == 'h') {
    ++*format;
    if (**format == 'h') {
      ++*format;
      length = DC166_SCAN_HH;
    } else {
      length = DC166_SCAN_H;
    }
  } else if (**format == 'j') {
    ++*format;
    length = DC166_SCAN_J;
  } else if (**format == 'z') {
    ++*format;
    length = DC166_SCAN_Z;
  } else if (**format == 't') {
    ++*format;
    length = DC166_SCAN_T;
  } else if (**format == 'L') {
    ++*format;
    length = DC166_SCAN_CAP_L;
  }
  return length;
}

static int scan_failure(int input_pending, int assignments)
{
  return input_pending ? EOF : assignments;
}

int _doscan(FILE *stream, const char *format, va_list arguments)
{
  dc166_scanner scanner;
  int assignments = 0;
  int input_pending = 1;

  scanner.stream = stream;
  scanner.count = 0;
  while (*format != 0) {
    int format_character = (unsigned char)*format++;

    if (format_character == ' ' || format_character == '\t') {
      int character;
      do {
        character = scan_get(&scanner);
      } while (scan_space(character));
      scan_push(&scanner, character);
      continue;
    }

    if (format_character != '%' || *format == '%') {
      int character;
      if (format_character == '%')
        ++format;
      character = scan_get(&scanner);
      if (character != format_character) {
        scan_push(&scanner, character);
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

      if (*format == '*') {
        suppress = 1;
        ++format;
      }
      while (scan_digit((unsigned char)*format)) {
        width = width * 10 + *format++ - '0';
      }
      length = scan_length(&format);
      conversion = (unsigned char)*format++;

      if (conversion == 'X')
        conversion = 'x';
      if (conversion == 'd' || conversion == 'i' || conversion == 'o' ||
          conversion == 'u' || conversion == 'x') {
        int result;
        if (!suppress)
          destination = va_arg(arguments, void *);
        result = scan_integer(&scanner, conversion, length, width,
                              destination);
        if (result == EOF)
          return scan_failure(input_pending, assignments);
        input_pending = 0;
        assignments += result;
        continue;
      }

      if (conversion == 'c') {
        int remaining = width == 0 ? 1 : width;
        int matched = 0;
        char *output = 0;

        if (!suppress)
          output = va_arg(arguments, char *);
        while (remaining-- != 0) {
          int character = scan_get(&scanner);
          if (character == EOF) {
            if (!suppress && matched != 0) {
              ++assignments;
              input_pending = 0;
            }
            return scan_failure(input_pending, assignments);
          }
          if (!suppress)
            output[matched] = (char)character;
          ++matched;
        }
        if (!suppress) {
          int character = scan_get(&scanner);
          scan_push(&scanner, character);
          ++assignments;
          input_pending = 0;
        }
        continue;
      }

      if (conversion == '[' || conversion == 's') {
        const char *set_begin = 0;
        const char *set_end = 0;
        int inverted = 0;
        int result;
        char *output = 0;

        if (conversion == '[') {
          length |= DC166_SCAN_SET;
          set_begin = format;
          if (*set_begin == '^') {
            inverted = 1;
            ++set_begin;
          }
          set_end = set_begin;
          if (*set_end == ']')
            ++set_end;
          while (*set_end != 0 && *set_end != ']')
            ++set_end;
          format = *set_end == ']' ? set_end + 1 : set_end;
        }
        if (!suppress)
          output = va_arg(arguments, char *);
        result = scan_string(&scanner, conversion == '[', set_begin, set_end,
                             inverted, width, output);
        if (result == EOF)
          return scan_failure(input_pending, assignments);
        if (result == 0 && conversion == '[' && !suppress)
          return assignments;
        assignments += result;
        input_pending = 0;
        continue;
      }

      if (conversion == 'p') {
        int result;
        if (!suppress)
          destination = va_arg(arguments, void *);
        result = scan_pointer(&scanner, length, destination);
        if (result == EOF)
          return scan_failure(input_pending, assignments);
        assignments += result;
        input_pending = 0;
        continue;
      }

      if (conversion == 'n') {
        if (!suppress) {
          destination = va_arg(arguments, void *);
          scan_store(destination, length, (unsigned long)scanner.count);
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
