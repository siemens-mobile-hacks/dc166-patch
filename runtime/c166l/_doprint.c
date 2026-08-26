#include "dc166_stdio.h"

#define DC166_LENGTH_DEFAULT 0
#define DC166_LENGTH_NEAR 1
#define DC166_LENGTH_LONG 2
#define DC166_LENGTH_HUGE 3

static void output_character(FILE *stream, int character, int *count)
{
  ++*count;
  fputc(character, stream);
}

static void output_string(FILE *stream, const char *string, int *count)
{
  while (*string != 0)
    output_character(stream, (int)*string++, count);
}

static void convert_unsigned(char *buffer, unsigned long value,
                             unsigned int base, int lower_case)
{
  char reverse[12];
  unsigned int length = 0U;
  unsigned int digit;

  do {
    digit = (unsigned int)(value % (unsigned long)base);
    value /= (unsigned long)base;
    if (digit < 10U)
      reverse[length++] = (char)('0' + digit);
    else
      reverse[length++] = (char)((lower_case ? 'a' : 'A') + digit - 10U);
  } while (value != 0UL);

  while (length != 0U)
    *buffer++ = reverse[--length];
  *buffer = 0;
}

static void output_integer(FILE *stream, int conversion, int length,
                           va_list *arguments, int *count)
{
  unsigned long magnitude;
  long signed_value;
  unsigned int base;
  int signed_conversion = conversion == 'd' || conversion == 'i';
  char buffer[13];

  if (length == DC166_LENGTH_LONG) {
    if (signed_conversion) {
      signed_value = va_arg(*arguments, long);
      if (signed_value < 0L) {
        output_character(stream, '-', count);
        magnitude = 0UL - (unsigned long)signed_value;
      } else {
        magnitude = (unsigned long)signed_value;
      }
    } else {
      magnitude = va_arg(*arguments, unsigned long);
    }
  } else {
    if (signed_conversion) {
      int value = va_arg(*arguments, int);
      if (value < 0) {
        output_character(stream, '-', count);
        magnitude = (unsigned long)(unsigned int)(-value);
      } else {
        magnitude = (unsigned long)(unsigned int)value;
      }
    } else {
      magnitude = (unsigned long)va_arg(*arguments, unsigned int);
    }
  }

  if (conversion == 'o')
    base = 8U;
  else if (conversion == 'x' || conversion == 'X')
    base = 16U;
  else
    base = 10U;
  convert_unsigned(buffer, magnitude, base, conversion == 'x');
  output_string(stream, buffer, count);
}

static void output_hex_byte(FILE *stream, unsigned char value, int *count)
{
  static const char digits[] = "0123456789ABCDEF";

  output_character(stream, digits[value >> 4], count);
  output_character(stream, digits[value & 15U], count);
}

static void output_pointer(FILE *stream, int length, va_list *arguments,
                           int *count)
{
#if _MODEL != 't'
  union {
    void _far *pointer;
    unsigned char bytes[4];
  } far_value;
  union {
    void _huge *pointer;
    unsigned char bytes[4];
  } huge_value;
#endif
  union {
    void _near *pointer;
    unsigned char bytes[2];
  } near_value;

#if _MODEL == 't'
  (void)length;
  near_value.pointer = va_arg(*arguments, void *);
  output_string(stream, "<near>", count);
  output_hex_byte(stream, near_value.bytes[1], count);
  output_hex_byte(stream, near_value.bytes[0], count);
#else
  if (length == DC166_LENGTH_NEAR) {
#if _MODEL == 'l' || _MODEL == 'm'
    far_value.pointer = va_arg(*arguments, void *);
    near_value.bytes[0] = far_value.bytes[0];
    near_value.bytes[1] = far_value.bytes[1];
    if (far_value.bytes[0] != 0U || far_value.bytes[1] != 0U ||
        far_value.bytes[2] != 0U || far_value.bytes[3] != 0U)
      near_value.bytes[1] |= 0x80U;
#elif _MODEL == 'h'
    huge_value.pointer = va_arg(*arguments, void *);
    near_value.pointer = (void _near *)huge_value.pointer;
#else
    near_value.pointer = va_arg(*arguments, void _near *);
#endif
    output_string(stream, "<near>", count);
    output_hex_byte(stream, near_value.bytes[1], count);
    output_hex_byte(stream, near_value.bytes[0], count);
  } else if (length == DC166_LENGTH_HUGE ||
             (_MODEL == 'h' && length == DC166_LENGTH_DEFAULT)) {
    huge_value.pointer = va_arg(*arguments, void _huge *);
    output_string(stream, "<huge>", count);
    output_hex_byte(stream, huge_value.bytes[2], count);
    output_character(stream, ':', count);
    output_hex_byte(stream, huge_value.bytes[1], count);
    output_hex_byte(stream, huge_value.bytes[0], count);
  } else {
    far_value.pointer = va_arg(*arguments, void _far *);
    output_string(stream, "<far>", count);
    output_hex_byte(stream, far_value.bytes[3], count);
    output_hex_byte(stream, far_value.bytes[2], count);
    output_character(stream, ':', count);
    output_hex_byte(stream, far_value.bytes[1], count);
    output_hex_byte(stream, far_value.bytes[0], count);
  }
#endif
}

int _doprint(FILE *stream, const char *format, va_list arguments)
{
  int count = 0;
  int conversion;
  int length;

  while (*format != 0) {
    if (*format != '%' || format[1] == '%') {
      output_character(stream, (int)*format++, &count);
      if (format[-1] == '%' && *format == '%')
        ++format;
      continue;
    }

    ++format;
    length = DC166_LENGTH_DEFAULT;
    if (*format == 'h') {
      length = DC166_LENGTH_NEAR;
      ++format;
    } else if (*format == 'l') {
      length = DC166_LENGTH_LONG;
      ++format;
    } else if (*format == 'L') {
      length = DC166_LENGTH_HUGE;
      ++format;
    }
    conversion = (unsigned char)*format++;

    if (conversion == 'c') {
      output_character(stream, (char)va_arg(arguments, int), &count);
    } else if (conversion == 'd' || conversion == 'i' ||
               conversion == 'o' || conversion == 'u' ||
               conversion == 'x' || conversion == 'X') {
      output_integer(stream, conversion, length, &arguments, &count);
    } else if (conversion == 's') {
      output_string(stream, va_arg(arguments, char *), &count);
    } else if (conversion == 'f' || conversion == 'F' ||
               conversion == 'e' || conversion == 'E' ||
               conversion == 'g' || conversion == 'G') {
      (void)va_arg(arguments, double);
      output_string(stream, "<no floats>", &count);
    } else if (conversion == 'p') {
      output_pointer(stream, length, &arguments, &count);
    } else if (conversion == 'n') {
      if (length == DC166_LENGTH_LONG)
        *va_arg(arguments, long *) = (long)count;
      else
        *va_arg(arguments, int *) = count;
    } else {
      errno = ERR_FORMAT;
      return count;
    }
  }
  return count;
}
