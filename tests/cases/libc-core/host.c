#include "test.h"

typedef unsigned int size_t;

extern void *memchr(const void *, int, size_t);
extern int memcmp(const void *, const void *, size_t);
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern void *memset(void *, int, size_t);
extern char *strcat(char *, const char *);
extern char *strchr(const char *, int);
extern int strcmp(const char *, const char *);
extern int strcoll(const char *, const char *);
extern char *strcpy(char *, const char *);
extern size_t strcspn(const char *, const char *);
extern size_t strlen(const char *);
extern char *strncat(char *, const char *, size_t);
extern int strncmp(const char *, const char *, size_t);
extern char *strncpy(char *, const char *, size_t);
extern char *strpbrk(const char *, const char *);
extern char *strrchr(const char *, int);
extern size_t strspn(const char *, const char *);
extern char *strstr(const char *, const char *);
extern char *strtok(char *, const char *);
extern size_t strxfrm(char *, const char *, size_t);
extern int isalnum(int);
extern int isalpha(int);
extern int isascii(int);
extern int iscntrl(int);
extern int isdigit(int);
extern int isgraph(int);
extern int islower(int);
extern int isprint(int);
extern int ispunct(int);
extern int isspace(int);
extern int isupper(int);
extern int isxdigit(int);
extern int toascii(int);
extern int tolower(int);
extern int toupper(int);
extern int rand(void);
extern void srand(unsigned int);

static char source[18];
static char buffer[48];
static char overlap[16];
static char transform[8];

static void test_strings(void)
{
  char *token;

  test_check(1U, strlen("abc") == 3U);
  test_check(2U, strcpy(buffer, "abc") == buffer && strcmp(buffer, "abc") == 0);
  test_check(3U, strcat(buffer, "def") == buffer && strcmp(buffer, "abcdef") == 0);
  test_check(4U, strncpy(buffer, "xy", 4U) == buffer &&
                 buffer[0] == 'x' && buffer[1] == 'y' &&
                 buffer[2] == 0 && buffer[3] == 0);
  buffer[0] = 'a'; buffer[1] = 0;
  test_check(5U, strncat(buffer, "bcdef", 2U) == buffer &&
                 strcmp(buffer, "abc") == 0);
  test_check(6U, strchr("abca", 'b') != 0 && *strchr("abca", 'b') == 'b');
  test_check(7U, strchr("abca", 0) != 0 && *strchr("abca", 0) == 0);
  test_check(8U, strrchr("abca", 'a') != 0 &&
                 strrchr("abca", 'a')[1] == 0);
  test_check(9U, strcmp("abc", "abd") < 0 && strcmp("abc", "abc") == 0);
  test_check(10U, strncmp("abc", "abd", 2U) == 0 &&
                  strncmp("abc", "abd", 3U) < 0);
  test_check(11U, strspn("abc123", "abc") == 3U);
  test_check(12U, strcspn("abc123", "0123456789") == 3U);
  test_check(13U, strpbrk("abcdef", "xdy") != 0 &&
                  *strpbrk("abcdef", "xdy") == 'd');
  test_check(14U, strstr("one two three", "two") != 0 &&
                  *strstr("one two three", "two") == 't');
  test_check(15U, strcoll("abc", "abd") < 0);
  test_check(16U, strxfrm(transform, "hello", sizeof(transform)) == 5U &&
                  strcmp(transform, "hello") == 0);

  strcpy(source, "alpha,beta;;gamma");
  token = strtok(source, ",;");
  test_check(17U, token != 0 && strcmp(token, "alpha") == 0);
  token = strtok(0, ",;");
  test_check(18U, token != 0 && strcmp(token, "beta") == 0);
  token = strtok(0, ",;");
  test_check(19U, token != 0 && strcmp(token, "gamma") == 0);
  test_check(20U, strtok(0, ",;") == 0);
}

static void test_memory(void)
{
  test_u16 index;

  memset(buffer, 0x5a, 10U);
  for (index = 0U; index < 10U; ++index)
    test_check(21U, buffer[index] == 'Z');
  test_check(22U, memcpy(buffer, "abcdef", 7U) == buffer &&
                  memcmp(buffer, "abcdef", 7U) == 0);
  test_check(23U, memchr(buffer, 'd', 7U) == buffer + 3);
  test_check(24U, memchr(buffer, 'x', 7U) == 0);
  strcpy(overlap, "0123456789");
  memmove(overlap + 2, overlap, 8U);
  test_check(25U, memcmp(overlap, "0101234567", 10U) == 0);
  memmove(overlap, overlap + 2, 8U);
  test_check(26U, memcmp(overlap, "01234567", 8U) == 0);
}

static void test_character_classes(void)
{
  test_check(27U, isalpha('A') && isupper('A') && !islower('A'));
  test_check(28U, isalpha('z') && islower('z') && !isupper('z'));
  test_check(29U, isdigit('7') && isxdigit('7') && isalnum('7'));
  test_check(30U, isxdigit('f') && isxdigit('B') && !isxdigit('g'));
  test_check(31U, isspace(' ') && isspace('\n') && iscntrl('\n'));
  test_check(32U, ispunct('!') && isgraph('!') && isprint('!'));
  test_check(33U, !isprint('\n') && !isalpha(-1) && !isdigit(256));
  test_check(34U, isascii(127) && !isascii(128) && toascii(0x1c1) == 0x41);
  test_check(35U, toupper('q') == 'Q' && toupper('!') == '!');
  test_check(36U, tolower('Q') == 'q' && tolower('!') == '!');
}

static void test_random(void)
{
  srand(1U);
  test_check(37U, rand() == 16838);
  test_check(38U, rand() == 5758);
  test_check(39U, rand() == 10113);
  srand(1U);
  test_check(40U, rand() == 16838);
}

void main(void)
{
  simulator_result = 0U;
  test_strings();
  test_memory();
  test_character_classes();
  test_random();
  simulator_stop();
}
