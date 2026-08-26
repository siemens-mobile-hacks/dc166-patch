#include "test.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


static char file_buffer[BUFSIZ];
static char text[64];

static int call_vfprintf(FILE *stream, const char *format, ...)
{
  va_list arguments;
  int result;

  va_start(arguments, format);
  result = vfprintf(stream, format, arguments);
  va_end(arguments);
  return result;
}

static int call_vprintf(const char *format, ...)
{
  va_list arguments;
  int result;

  va_start(arguments, format);
  result = vprintf(format, arguments);
  va_end(arguments);
  return result;
}

static void test_file_stream(void)
{
  const char *name = "dc166-fprintf.txt";
  FILE *stream;

  (void)remove(name);
  stream = fopen(name, "w+b");
  test_check(1U, stream != 0);
  if (stream == 0)
    return;
  setbuf(stream, file_buffer);
  test_check(2U, fprintf(stream, "A:%d", 12) == 4);
  test_check(3U, call_vfprintf(stream, "|B:%lx", 0x1234abcdUL) == 11);
  test_check(4U, fflush(stream) == 0 && fseek(stream, 0L, SEEK_SET) == 0);
  memset(text, 0, sizeof(text));
  test_check(5U, fread(text, 1U, 15U, stream) == 15U &&
                 memcmp(text, "A:12|B:1234abcd", 15U) == 0);
  test_check(6U, fclose(stream) == 0 && remove(name) == 0);
}

static void test_stdout_stream(void)
{
  const char *name = "dc166-printf.txt";
  FILE *stream;

  (void)remove(name);
  test_check(7U, freopen(name, "wb", stdout) == stdout);
  test_check(8U, printf("P:%u", 42U) == 4);
  test_check(9U, call_vprintf("|V:%s", "ok") == 5);
  test_check(10U, fclose(stdout) == 0);

  stream = fopen(name, "rb");
  test_check(11U, stream != 0);
  if (stream != 0) {
    memset(text, 0, sizeof(text));
    test_check(12U, fread(text, 1U, 9U, stream) == 9U &&
                    memcmp(text, "P:42|V:ok", 9U) == 0);
    fclose(stream);
  }
  test_check(13U, remove(name) == 0);
}

void main(void)
{
  simulator_result = 0U;
  test_file_stream();
  test_stdout_stream();
  simulator_stop();
}
