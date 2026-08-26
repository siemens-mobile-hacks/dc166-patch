#include "test.h"
#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#undef iswalnum
#undef iswalpha
#undef iswcntrl
#undef iswdigit
#undef iswgraph
#undef iswlower
#undef iswprint
#undef iswpunct
#undef iswspace
#undef iswupper
#undef iswxdigit
#undef towlower
#undef towupper

extern int iswalnum(wint_t);
extern int iswalpha(wint_t);
extern int iswcntrl(wint_t);
extern int iswdigit(wint_t);
extern int iswgraph(wint_t);
extern int iswlower(wint_t);
extern int iswprint(wint_t);
extern int iswpunct(wint_t);
extern int iswspace(wint_t);
extern int iswupper(wint_t);
extern int iswxdigit(wint_t);
extern wint_t towlower(wint_t);
extern wint_t towupper(wint_t);

static wchar_t first[40];
static wchar_t second[40];
static wchar_t overlap[20];
static wchar_t converted_wide[20];
static char converted_bytes[20];

static void test_wide_strings(void)
{
  wchar_t *context;
  wchar_t *token;

  test_check(1U, wcscpy(first, L"alpha") == first && wcslen(first) == 5U);
  test_check(2U, wcscat(first, L" beta") == first &&
                 wcscmp(first, L"alpha beta") == 0);
  test_check(3U, wcsncpy(second, L"xy", 4U) == second &&
                 second[0] == 'x' && second[1] == 'y' &&
                 second[2] == 0 && second[3] == 0);
  wcscpy(second, L"a");
  wcsncat(second, L"bcdef", 2U);
  test_check(4U, wcscmp(second, L"abc") == 0);
  test_check(5U, wcscmp(L"abc", L"abd") < 0 &&
                 wcsncmp(L"abc", L"abd", 2U) == 0);
  test_check(6U, wcscoll(L"abc", L"abd") < 0);
  test_check(7U, wcsxfrm(second, L"hello", 40U) == 5U &&
                 wcscmp(second, L"hello") == 0);
  wcscpy(first, L"abca");
  test_check(8U, wcschr(first, 'b') == first + 1U);
  test_check(9U, wcsrchr(first, 'a') == first + 3U);
  test_check(10U, wcsspn(L"abc123", L"abc") == 3U &&
                  wcscspn(L"abc123", L"0123456789") == 3U);
  test_check(11U, *wcspbrk(L"abcdef", L"xdy") == 'd');
  wcscpy(first, L"one two");
  test_check(12U, wcsstr(first, L"two") == first + 4U);

  wcscpy(first, L"one,two;three");
  context = 0;
  token = wcstok(first, L",;", &context);
  test_check(13U, token != 0 && wcscmp(token, L"one") == 0);
  token = wcstok(0, L",;", &context);
  test_check(14U, token != 0 && wcscmp(token, L"two") == 0);
  token = wcstok(0, L",;", &context);
  test_check(15U, token != 0 && wcscmp(token, L"three") == 0);
  test_check(16U, wcstok(0, L",;", &context) == 0);
}

static void test_wide_memory(void)
{
  wmemset(first, 'Q', 8U);
  test_check(17U, first[0] == 'Q' && first[7] == 'Q');
  wmemcpy(second, L"abcdef", 7U);
  test_check(18U, wmemcmp(second, L"abcdef", 7U) == 0);
  test_check(19U, wmemchr(second, 'd', 7U) == second + 3U);
  wcscpy(overlap, L"0123456789");
  wmemmove(overlap + 2, overlap, 8U);
  test_check(20U, wmemcmp(overlap, L"0101234567", 10U) == 0);
  wmemmove(overlap, overlap + 2, 8U);
  test_check(21U, wmemcmp(overlap, L"01234567", 8U) == 0);
}

static void test_wide_classes(void)
{
  wctype_t alpha = wctype("alpha");
  wctrans_t upper = wctrans("toupper");
  test_check(22U, alpha == _alpha && iswctype('A', alpha));
  test_check(23U, iswalpha('z') && iswalnum('7') && iswdigit('7'));
  test_check(24U, iswupper('A') && iswlower('a') && iswxdigit('f'));
  test_check(25U, iswspace('\n') && iswcntrl('\n'));
  test_check(26U, iswpunct('!') && iswgraph('!') && iswprint(' '));
  test_check(27U, !iswalpha(WEOF) && !iswalpha(0x100U));
  test_check(28U, towlower('Q') == 'q' && towupper('q') == 'Q');
  test_check(29U, upper == _to_upper && towctrans('x', upper) == 'X');
  test_check(30U, wctype("missing") == 0U && wctrans("missing") == 0U);
}

static void test_multibyte(void)
{
  mbstate_t state;
  wchar_t wide;
  const char *byte_source;
  const wchar_t *wide_source;

  state.wc_value = 1;
  state.n_bytes = 1U;
  state.encoding = 1U;
  test_check(31U, !mbsinit(&state));
  state.wc_value = 0;
  state.n_bytes = 0U;
  state.encoding = 0U;
  test_check(32U, mbrtowc(&wide, "A", 1U, &state) == 1U &&
                  wide == 'A' && mbsinit(&state));
  test_check(33U, mbrtowc(&wide, "", 1U, &state) == 0U && wide == 0);
  test_check(34U, mbrlen("A", 0U, &state) == (size_t)-2);
  test_check(35U, wcrtomb(converted_bytes, 'Z', &state) == 1U &&
                  converted_bytes[0] == 'Z');
  errno = 0;
  test_check(36U, wcrtomb(converted_bytes, 0x100, &state) == (size_t)-1 &&
                  errno == EILSEQ);
  test_check(37U, mbtowc(&wide, "B", 1U) == 1 && wide == 'B');
  test_check(38U, wctomb(converted_bytes, 'C') == 1 &&
                  converted_bytes[0] == 'C');
  test_check(39U, mblen("", 1U) == 0 && mblen("D", 1U) == 1);

  byte_source = "hello";
  test_check(40U, mbsrtowcs(converted_wide, &byte_source, 20U, &state) == 5U &&
                  byte_source == 0 && wcscmp(converted_wide, L"hello") == 0);
  wide_source = L"world";
  test_check(41U, wcsrtombs(converted_bytes, &wide_source, 20U, &state) == 5U &&
                  wide_source == 0 && strcmp(converted_bytes, "world") == 0);
  test_check(42U, mbstowcs(converted_wide, "abc", 20U) == 3U &&
                  wcscmp(converted_wide, L"abc") == 0);
  test_check(43U, wcstombs(converted_bytes, L"xyz", 20U) == 3U &&
                  strcmp(converted_bytes, "xyz") == 0);
}

static void test_wide_numbers_and_locale(void)
{
  wchar_t *end;
  struct lconv *convention;
  long value = wcstol(L" -123x", &end, 10);
  test_check(44U, value == -123L && *end == 'x');
  test_check(45U, wcstoul(L"0xffffffff", 0, 0) == 0xffffffffUL);
  errno = 0;
  test_check(46U, wcstol(L"2147483648", 0, 10) == 2147483647L &&
                  errno == ERANGE);
  test_check(47U, strcmp(setlocale(LC_ALL, 0), "C") == 0 &&
                  strcmp(setlocale(LC_ALL, ""), "C") == 0);
  test_check(48U, setlocale(LC_ALL, "unsupported") == 0);
  convention = localeconv();
  test_check(49U, strcmp(convention->decimal_point, ".") == 0 &&
                  convention->thousands_sep[0] == 0);
  test_check(50U, strcmp(strerror(ERANGE), "Result too large") == 0 &&
                  strerror(EILSEQ) == 0);
}

void main(void)
{
  simulator_result = 0U;
  test_wide_strings();
  test_wide_memory();
  test_wide_classes();
  test_multibyte();
  test_wide_numbers_and_locale();
  simulator_stop();
}
