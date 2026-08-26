#include "test.h"
#include <stdio.h>
#include <string.h>


static char text[24];

static void test_file_stream(void)
{
  const char *name = "dc166-fscanf.txt";
  FILE *stream;
  int value = 0;

  (void)remove(name);
  stream = fopen(name, "w+b");
  test_check(1U, stream != 0);
  if (stream == 0)
    return;
  test_check(2U, fputs("123 alpha", stream) == 0 &&
                 fseek(stream, 0L, SEEK_SET) == 0);
  test_check(3U, fscanf(stream, "%d %s", &value, text) == 2 &&
                 value == 123 && strcmp(text, "alpha") == 0);
  test_check(4U, fclose(stream) == 0 && remove(name) == 0);
}

static void test_stdin_stream(void)
{
  const char *name = "dc166-scanf.txt";
  FILE *stream;
  unsigned int value = 0U;

  (void)remove(name);
  stream = fopen(name, "wb");
  test_check(5U, stream != 0);
  if (stream != 0)
    test_check(6U, fputs("2a beta", stream) == 0 && fclose(stream) == 0);

  test_check(7U, freopen(name, "rb", stdin) == stdin);
  test_check(8U, scanf("%x %s", &value, text) == 2 && value == 0x2aU &&
                 strcmp(text, "beta") == 0);
  test_check(9U, fclose(stdin) == 0 && remove(name) == 0);
}

void main(void)
{
  simulator_result = 0U;
  test_file_stream();
  test_stdin_stream();
  simulator_stop();
}
