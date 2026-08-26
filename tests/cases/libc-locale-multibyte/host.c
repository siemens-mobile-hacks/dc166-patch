#include "test.h"
#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>


static wchar_t wide_buffer[16];
static wchar_t invalid_wide[3];
static char byte_buffer[16];

static void clear_state(mbstate_t *state)
{
  state->wc_value = 0;
  state->n_bytes = 0U;
  state->encoding = 0U;
}

static void dirty_state(mbstate_t *state)
{
  state->wc_value = 0x1234;
  state->n_bytes = 1U;
  state->encoding = 1U;
}

static int state_is_dirty(const mbstate_t *state)
{
  return state->wc_value == 0x1234 &&
      state->n_bytes == 1U && state->encoding == 1U;
}

static void test_locale_and_errors(void)
{
  struct lconv *value;

  test_check(1U, strcmp(setlocale(-1, 0), "C") == 0);
  test_check(2U, strcmp(setlocale(99, "C"), "C") == 0);
  test_check(3U, strcmp(setlocale(LC_ALL, ""), "C") == 0);
  test_check(4U, setlocale(LC_ALL, "POSIX") == 0);

  value = localeconv();
  test_check(5U, strcmp(value->decimal_point, ".") == 0 &&
                 value->thousands_sep[0] == 0 && value->grouping[0] == 0);
  test_check(6U, value->int_frac_digits == 127 &&
                 value->frac_digits == 127 && value->n_sign_posn == 127);

  test_check(7U, strcmp(strerror(0), "No error") == 0);
  test_check(8U, strcmp(strerror(25),
                        "illegal format string for printf/scanf") == 0);
  test_check(9U, strcmp(strerror(26),
                        "floating point not supported") == 0);
  test_check(10U, strcmp(strerror(27), "long not supported") == 0 &&
                  strcmp(strerror(28), "pointers not supported") == 0);
  test_check(11U, strcmp(strerror(29), "Positioning failure") == 0);
  test_check(12U, strerror(EILSEQ) == 0);
}

static void test_legacy_multibyte(void)
{
  mbstate_t state;
  wchar_t wide;
  char high_byte[2];

  high_byte[0] = (char)0x80;
  high_byte[1] = 0;

  test_check(13U, mbsinit(0));
  clear_state(&state);
  test_check(14U, mbsinit(&state));
  dirty_state(&state);
  test_check(15U, !mbsinit(&state));

  wide = 7;
  test_check(16U, mbtowc(&wide, high_byte, 1U) == 1 && wide == -128);
  test_check(17U, mbtowc(&wide, "A", 0U) == 0 && wide == -128);
  test_check(18U, mbtowc(&wide, 0, 1U) == 0 && wide == -128);
  test_check(19U, mblen("A", 1U) == 1 && mblen("", 1U) == 0);

  test_check(20U, wctomb(0, 'A') == 0);
  test_check(21U, wctomb(byte_buffer, 0x1234) == 1 &&
                  (unsigned char)byte_buffer[0] == 0x34U);

  clear_state(&state);
  test_check(22U, mbrtowc(&wide, "A", 1U, &state) == 1U &&
                  wide == 'A' && mbsinit(&state));
  test_check(23U, mbrtowc(&wide, "", 1U, &state) == 0U &&
                  wide == 0 && mbsinit(&state));
  test_check(24U, mbrtowc(&wide, "A", 0U, &state) == (size_t)-2);
  test_check(25U, mbrtowc(&wide, high_byte, 1U, &state) == 1U &&
                  wide == -128 && mbsinit(&state));

  dirty_state(&state);
  test_check(26U, mbrtowc(&wide, "A", 1U, &state) == (size_t)-2 &&
                  state_is_dirty(&state));
  test_check(27U, mbrtowc(&wide, 0, 1U, &state) == 0U &&
                  state_is_dirty(&state));
  test_check(28U, mbrtowc(0, "", 1U, &state) == 0U &&
                  state_is_dirty(&state));
  test_check(29U, mbrlen(high_byte, 1U, 0) == 1U);

  dirty_state(&state);
  test_check(30U, wcrtomb(0, 0x1234, &state) == 1U && mbsinit(&state));
  dirty_state(&state);
  errno = 0;
  test_check(31U, wcrtomb(byte_buffer, 0x100, &state) == (size_t)-1 &&
                  errno == EILSEQ && state_is_dirty(&state));
  clear_state(&state);
  test_check(32U, wcrtomb(byte_buffer, 0xff, &state) == 1U &&
                  (unsigned char)byte_buffer[0] == 0xffU && mbsinit(&state));
}

static void test_restartable_strings(void)
{
  mbstate_t state;
  const char *bytes;
  const char *bytes_start;
  const wchar_t *wide;
  const wchar_t *wide_start;

  clear_state(&state);
  bytes = "hello";
  test_check(33U, mbsrtowcs(wide_buffer, &bytes, 16U, &state) == 5U &&
                  bytes == 0 && wcscmp(wide_buffer, L"hello") == 0 &&
                  mbsinit(&state));

  bytes_start = "abc";
  bytes = bytes_start;
  test_check(34U, mbsrtowcs(wide_buffer, &bytes, 2U, &state) == 2U &&
                  bytes == bytes_start + 2 &&
                  wide_buffer[0] == 'a' && wide_buffer[1] == 'b');
  bytes = bytes_start;
  test_check(35U, mbsrtowcs(0, &bytes, 2U, &state) == 2U &&
                  bytes == bytes_start + 2);
  bytes = bytes_start;
  test_check(36U, mbsrtowcs(wide_buffer, &bytes, 0U, &state) == 0U &&
                  bytes == 0);

  dirty_state(&state);
  bytes_start = "A";
  bytes = bytes_start;
  test_check(37U, mbsrtowcs(wide_buffer, &bytes, 1U, &state) == (size_t)-1 &&
                  bytes == bytes_start && state_is_dirty(&state));

  clear_state(&state);
  wide = L"world";
  test_check(38U, wcsrtombs(byte_buffer, &wide, 16U, &state) == 5U &&
                  wide == 0 && strcmp(byte_buffer, "world") == 0 &&
                  mbsinit(&state));

  wide_start = L"abc";
  wide = wide_start;
  test_check(39U, wcsrtombs(byte_buffer, &wide, 2U, &state) == 2U &&
                  wide == wide_start + 2 &&
                  byte_buffer[0] == 'a' && byte_buffer[1] == 'b');
  wide = wide_start;
  test_check(40U, wcsrtombs(0, &wide, 2U, &state) == 2U &&
                  wide == wide_start + 2);
  wide = wide_start;
  test_check(41U, wcsrtombs(byte_buffer, &wide, 0U, &state) == 0U &&
                  wide == wide_start);

  errno = 0;
  invalid_wide[0] = 'A';
  invalid_wide[1] = 0x100;
  invalid_wide[2] = 0;
  wide = invalid_wide;
  test_check(42U, wcsrtombs(byte_buffer, &wide, 3U, &state) == (size_t)-1);
  test_check(43U, errno == EILSEQ);
  test_check(44U, wide == invalid_wide);
}

static void test_nonrestartable_strings(void)
{
  static const wchar_t truncation[] = { 0x1234, 0 };

  test_check(45U, mbstowcs(wide_buffer, "abc", 16U) == 3U &&
                  wcscmp(wide_buffer, L"abc") == 0);
  test_check(46U, wcstombs(byte_buffer, truncation, 2U) == 1U &&
                  (unsigned char)byte_buffer[0] == 0x34U &&
                  byte_buffer[1] == 0);
}

void main(void)
{
  simulator_result = 0U;
  test_locale_and_errors();
  test_legacy_multibyte();
  test_restartable_strings();
  test_nonrestartable_strings();
  simulator_stop();
}
