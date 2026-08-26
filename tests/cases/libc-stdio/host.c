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


static unsigned char pattern[320];
static unsigned char copy[320];
static char full_buffer[19];

static void make_pattern(void)
{
  unsigned int index;

  for (index = 0U; index < sizeof(pattern); ++index)
    pattern[index] = (unsigned char)(index * 29U + 7U);
  memset(copy, 0, sizeof(copy));
}

static int same_bytes(const unsigned char *left,
                      const unsigned char *right,
                      unsigned int count)
{
  unsigned int index;

  for (index = 0U; index < count; ++index)
    if (left[index] != right[index])
      return 0;
  return 1;
}

static void test_initial_state_and_names(void)
{
  char first[L_tmpnam];
  char second[L_tmpnam];
  FILE closed_stream;
  FILE *temporary;

  test_check(1U, stdin->_flag == (_IORD | _IOLBF) &&
                 stdin->_bufsiz == 80 && stdin->_base != 0 &&
                 stdin->_ptr == stdin->_base && stdin->_file == 0);
  test_check(2U, stdout->_flag == (_IOWR | _IOLBF) &&
                 stdout->_bufsiz == 80 && stdout->_base != 0 &&
                 stdout->_ptr == stdout->_base && stdout->_file == 1);
  test_check(3U, stderr->_flag == (_IOWR | _IONBF) &&
                 stderr->_bufsiz == -1 && stderr->_base == 0 &&
                 stderr->_file == 2);

  test_check(4U, tmpnam(first) == first && strcmp(first, "tmp00001") == 0);
  test_check(5U, tmpnam(second) == second && strcmp(second, "tmp00002") == 0);

  memset(&closed_stream, 0x5a, sizeof(closed_stream));
  closed_stream._flag = 0U;
  errno = 0;
  test_check(6U, fclose(&closed_stream) == EOF && errno == EINVAL &&
                 closed_stream._flag == 0U && closed_stream._ptr == 0 &&
                 closed_stream._base == 0 && closed_stream._cnt == 0);

  temporary = tmpfile();
  test_check(7U, temporary != 0);
  if (temporary != 0)
    test_check(8U, fclose(temporary) == 0);
  test_check(9U, remove("tmp00003") == 0);
}

static void test_full_buffered_file(void)
{
  const char *name = "dc166-stdio-main.bin";
  const char *second = "dc166-stdio-reopen.bin";
  FILE *stream;
  fpos_t position;
  int character;

  (void)remove(name);
  (void)remove(second);
  make_pattern();

  stream = fopen(name, "w+b");
  test_check(10U, stream != 0);
  if (stream == 0)
    return;
  test_check(57U, (stream->_flag & _IOWR) != 0U);
  test_check(58U, (stream->_flag & _IORD) != 0U);
  test_check(59U, (stream->_flag & _IOMYBUF) != 0U);
  test_check(60U, (stream->_flag & _IONBF) == 0U &&
                  stream->_bufsiz == BUFSIZ);

  test_check(11U, setvbuf(stream, full_buffer, _IOFBF,
                          sizeof(full_buffer)) == 0 &&
                  stream->_base == full_buffer && stream->_bufsiz == 19 &&
                  (stream->_flag & (_IONBF | _IOLBF | _IOMYBUF)) == 0U);
  test_check(12U, fwrite(pattern, 4U, 10U, stream) == 10U);
  test_check(13U, ftell(stream) == 40L);
  test_check(14U, fgetpos(stream, &position) == 0 && position == 40UL);
  test_check(15U, fseek(stream, 0L, SEEK_SET) == 0);
  test_check(16U, fread(copy, 4U, 9U, stream) == 9U &&
                  same_bytes(pattern, copy, 36U));
  character = fgetc(stream);
  test_check(17U, character == (int)pattern[36]);
  test_check(18U, ungetc('Q', stream) == 'Q' && getc(stream) == 'Q');
  test_check(19U, fsetpos(stream, &position) == 0 && ftell(stream) == 40L);
  test_check(20U, (rewind(stream), ftell(stream) == 0L));

  while (fgetc(stream) != EOF)
    ;
  test_check(21U, feof(stream) && !ferror(stream));
  clearerr(stream);
  test_check(22U, !feof(stream) && !ferror(stream));

  test_check(23U, freopen(second, "w+b", stream) == stream &&
                  (stream->_flag & (_IORD | _IOWR | _IOMYBUF)) ==
                      (_IORD | _IOWR | _IOMYBUF) &&
                  (stream->_flag & _IONBF) == 0U &&
                  stream->_bufsiz == BUFSIZ);
  test_check(24U, fputc(0xa5, stream) == 0xa5 && fflush(stream) == 0);
  test_check(25U, fclose(stream) == 0);
  test_check(26U, remove(name) == 0 && remove(second) == 0);
}

void main(void)
{
  simulator_result = 0U;
  test_initial_state_and_names();
  test_full_buffered_file();
  cleanup();
  test_check(27U, _iob[0]._flag == 0U && _iob[9]._flag == 0U);
  simulator_stop();
}
