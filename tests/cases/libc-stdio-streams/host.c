#include "test.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#undef getc
#undef getchar
#undef putc
#undef putchar
#undef clearerr
#undef feof
#undef ferror

extern void cleanup(void);


static char line_buffer[11];
static char input_buffer[13];
static char setbuf_buffer[BUFSIZ];
static char text[96];

static void test_line_buffering(void)
{
  const char *name = "dc166-stdio-line.txt";
  FILE *stream;

  (void)remove(name);
  stream = fopen(name, "w+b");
  test_check(1U, stream != 0);
  if (stream == 0)
    return;
  test_check(2U, setvbuf(stream, line_buffer, _IOLBF,
                         sizeof(line_buffer)) == 0);
  test_check(3U, fputs("abc", stream) == 0 &&
                 stream->_ptr - stream->_base == 3);
  test_check(4U, fputc('\n', stream) == '\n' &&
                 stream->_ptr == stream->_base);
  test_check(5U, fputs("tail", stream) == 0 && fflush(stream) == 0 &&
                 ftell(stream) == 8L);
  test_check(6U, fclose(stream) == 0 && remove(name) == 0);
}

static void test_standard_stream_functions(void)
{
  const char *input_name = "dc166-stdio-input.txt";
  const char *output_name = "dc166-stdio-output.txt";
  const char *error_name = "dc166-stdio-error.txt";
  FILE *stream;

  (void)remove(input_name);
  (void)remove(output_name);
  (void)remove(error_name);

  stream = fopen(input_name, "wb");
  test_check(7U, stream != 0);
  if (stream != 0)
    test_check(8U, fputs("Zone\ntwo", stream) == 0 && fclose(stream) == 0);

  test_check(9U, freopen(input_name, "rb", stdin) == stdin);
  test_check(10U, setvbuf(stdin, input_buffer, _IOLBF,
                          sizeof(input_buffer)) == 0);
  test_check(11U, getchar() == 'Z');
  test_check(12U, fgets(text, sizeof(text), stdin) == text &&
                  strcmp(text, "one\n") == 0);
  test_check(13U, gets(text) == 0);
  test_check(14U, fclose(stdin) == 0 && remove(input_name) == 0);

  test_check(15U, freopen(output_name, "wb", stdout) == stdout);
  setbuf(stdout, setbuf_buffer);
  test_check(16U, putc('A', stdout) == 'A' && putchar('B') == 'B' &&
                  puts("C") == 0 && fflush(0) == 0);
  test_check(17U, fclose(stdout) == 0);

  stream = fopen(output_name, "rb");
  test_check(18U, stream != 0);
  if (stream != 0) {
    memset(text, 0, sizeof(text));
    test_check(19U, fread(text, 1U, 4U, stream) == 4U &&
                    memcmp(text, "ABC\n", 4U) == 0);
    fclose(stream);
  }
  test_check(20U, remove(output_name) == 0);

  test_check(21U, freopen(error_name, "wb", stderr) == stderr);
  setbuf(stderr, 0);
  errno = ENOENT;
  perror("demo");
  test_check(22U, fclose(stderr) == 0);
  stream = fopen(error_name, "rb");
  test_check(23U, stream != 0);
  if (stream != 0) {
    memset(text, 0, sizeof(text));
    test_check(24U, fread(text, 1U, 32U, stream) == 32U &&
                    memcmp(text, "demo: No such file or directory\n", 32U) == 0);
    fclose(stream);
  }
  test_check(25U, remove(error_name) == 0);
}

static void test_memory_stream_and_modes(void)
{
  FILE stream;
  char buffer[4];

  memset(&stream, 0, sizeof(stream));
  stream._ptr = buffer;
  stream._base = buffer;
  stream._cnt = 2;
  stream._bufsiz = 2;
  stream._flag = _IOWR | _IOSTRG;
  test_check(26U, fputc('x', &stream) == 'x' &&
                  fputc('y', &stream) == 'y' &&
                  fputc('z', &stream) == EOF &&
                  buffer[0] == 'x' && buffer[1] == 'y');

  memset(&stream, 0, sizeof(stream));
  stream._flag = _IOWR;
  test_check(27U, setvbuf(&stream, buffer, 0x0c, sizeof(buffer)) == 1);
  test_check(28U, setvbuf(&stream, buffer, _IONBF, sizeof(buffer)) == 1);
  test_check(29U, setvbuf(&stream, 0, _IOFBF, 0U) == 1 &&
                  (stream._flag & _IONBF) != 0U &&
                  stream._bufsiz == -1);
}

void main(void)
{
  simulator_result = 0U;
  test_line_buffering();
  test_standard_stream_functions();
  test_memory_stream_and_modes();
  cleanup();
  test_check(30U, _iob[0]._flag == 0U && _iob[9]._flag == 0U);
  simulator_stop();
}
