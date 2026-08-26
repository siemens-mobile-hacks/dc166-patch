#include "dc166_stdio.h"

#define DC166_WLEN_DEFAULT 0
#define DC166_WLEN_NEAR 1
#define DC166_WLEN_LONG 2
#define DC166_WLEN_HUGE 3

static void wide_character(FILE *stream, wchar_t character, int *count)
{
  ++*count;
  fputwc(character, stream);
}

static void wide_string(FILE *stream, const wchar_t *string, int *count)
{
  while (*string != 0)
    wide_character(stream, *string++, count);
}

static void ascii_string(FILE *stream, const char *string, int *count)
{
  while (*string != 0)
    wide_character(stream, (wchar_t)(unsigned char)*string++, count);
}

static void wide_unsigned(wchar_t *buffer, unsigned long value,
                          unsigned int base, int lower_case)
{
  wchar_t reverse[12];
  unsigned int length = 0U;
  unsigned int digit;

  do {
    digit = (unsigned int)(value % (unsigned long)base);
    value /= (unsigned long)base;
    if (digit < 10U)
      reverse[length++] = (wchar_t)('0' + digit);
    else
      reverse[length++] = (wchar_t)((lower_case ? 'a' : 'A') + digit - 10U);
  } while (value != 0UL);
  while (length != 0U)
    *buffer++ = reverse[--length];
  *buffer = 0;
}

static void wide_integer(FILE *stream, int conversion, int length,
                         va_list *arguments, int *count)
{
  unsigned long magnitude;
  long signed_value;
  int signed_conversion = conversion == 'd' || conversion == 'i';
  unsigned int base;
  wchar_t buffer[13];

  if (length == DC166_WLEN_LONG || length == DC166_WLEN_HUGE) {
    if (signed_conversion) {
      signed_value = va_arg(*arguments, long);
      if (signed_value < 0L) {
        wide_character(stream, '-', count);
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
        wide_character(stream, '-', count);
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
  wide_unsigned(buffer, magnitude, base, conversion == 'x');
  wide_string(stream, buffer, count);
}

static void wide_hex_byte(FILE *stream, unsigned char value, int *count)
{
  static const wchar_t digits[] = L"0123456789ABCDEF";
  wide_character(stream, digits[value >> 4], count);
  wide_character(stream, digits[value & 15U], count);
}

static void wide_pointer(FILE *stream, int length, va_list *arguments,
                         int *count)
{
#if _MODEL != 't'
  union { void _far *pointer; unsigned char bytes[4]; } far_value;
  union { void _huge *pointer; unsigned char bytes[4]; } huge_value;
#endif
  union { void _near *pointer; unsigned char bytes[2]; } near_value;

#if _MODEL == 't'
  (void)length;
  near_value.pointer = va_arg(*arguments, void *);
  ascii_string(stream, "<near>", count);
  wide_hex_byte(stream, near_value.bytes[1], count);
  wide_hex_byte(stream, near_value.bytes[0], count);
#else
  if (length == DC166_WLEN_NEAR) {
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
    ascii_string(stream, "<near>", count);
    wide_hex_byte(stream, near_value.bytes[1], count);
    wide_hex_byte(stream, near_value.bytes[0], count);
  } else if (length == DC166_WLEN_HUGE ||
             (_MODEL == 'h' && length == DC166_WLEN_DEFAULT)) {
    huge_value.pointer = va_arg(*arguments, void _huge *);
    ascii_string(stream, "<huge>", count);
    wide_hex_byte(stream, huge_value.bytes[2], count);
    wide_character(stream, ':', count);
    wide_hex_byte(stream, huge_value.bytes[1], count);
    wide_hex_byte(stream, huge_value.bytes[0], count);
  } else {
    far_value.pointer = va_arg(*arguments, void _far *);
    ascii_string(stream, "<far>", count);
    wide_hex_byte(stream, far_value.bytes[3], count);
    wide_hex_byte(stream, far_value.bytes[2], count);
    wide_character(stream, ':', count);
    wide_hex_byte(stream, far_value.bytes[1], count);
    wide_hex_byte(stream, far_value.bytes[0], count);
  }
#endif
}

int _dowprint(FILE *stream, const wchar_t *format, va_list arguments)
{
  int count = 0;

  while (*format != 0) {
    int conversion;
    int length = DC166_WLEN_DEFAULT;

    if (*format != '%' || format[1] == '%') {
      wide_character(stream, *format++, &count);
      if (format[-1] == '%' && *format == '%')
        ++format;
      continue;
    }
    ++format;
    if (*format == 'h') {
      length = DC166_WLEN_NEAR;
      ++format;
    } else if (*format == 'l') {
      length = DC166_WLEN_LONG;
      ++format;
    } else if (*format == 'L') {
      length = DC166_WLEN_HUGE;
      ++format;
    }
    conversion = *format++;

    if (conversion == 'c') {
      int value = va_arg(arguments, int);
      wide_character(stream, length == DC166_WLEN_LONG ?
                     (wchar_t)value : (wchar_t)btowc(value), &count);
    } else if (conversion == 'd' || conversion == 'i' ||
               conversion == 'o' || conversion == 'u' ||
               conversion == 'x' || conversion == 'X') {
      wide_integer(stream, conversion, length, &arguments, &count);
    } else if (conversion == 's') {
      wide_string(stream, va_arg(arguments, wchar_t *), &count);
    } else if (conversion == 'f' || conversion == 'F' ||
               conversion == 'e' || conversion == 'E' ||
               conversion == 'g' || conversion == 'G') {
      (void)va_arg(arguments, double);
      ascii_string(stream, "<no floats>", &count);
    } else if (conversion == 'p') {
      wide_pointer(stream, length, &arguments, &count);
    } else if (conversion == 'n') {
      if (length == DC166_WLEN_LONG)
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
