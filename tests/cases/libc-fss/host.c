#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


static unsigned char write_buffer[320];
static unsigned char read_buffer[320];
static char current_directory[128];
static char long_name[256];

static void fill_pattern(void)
{
  unsigned int index;

  for (index = 0U; index < sizeof(write_buffer); ++index)
    write_buffer[index] = (unsigned char)(index * 37U + 11U);
  memset(read_buffer, 0, sizeof(read_buffer));
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

static void test_files(void)
{
  const char *first = "dc166-fss-a.bin";
  const char *second = "dc166-fss-b.bin";
  struct stat status;
  int descriptor;

  (void)unlink(first);
  (void)unlink(second);
  fill_pattern();

  descriptor = open(first, O_CREAT | O_TRUNC | O_RDWR | O_BINARY);
  test_check(1U, descriptor >= 0);
  test_check(2U, write(descriptor, (const char *)write_buffer, 31U) == 31U);
  test_check(3U, lseek(descriptor, 0L, 0) == 0L);
  test_check(4U, read(descriptor, (char *)read_buffer, 31U) == 31U &&
                 same_bytes(write_buffer, read_buffer, 31U));

  test_check(5U, lseek(descriptor, 0L, 0) == 0L);
  test_check(6U, write(descriptor, (const char *)write_buffer, 300U) == 300U);
  test_check(7U, lseek(descriptor, 0L, 0) == 0L);
  memset(read_buffer, 0, sizeof(read_buffer));
  test_check(8U, read(descriptor, (char *)read_buffer, 300U) == 300U);
  test_check(27U, same_bytes(write_buffer, read_buffer, 300U));
  test_check(9U, close(descriptor) == 0);

  memset(&status, 0x5a, sizeof(status));
  test_check(10U, stat(first, &status) == 0);
  test_check(25U, status.st_size != 0UL);
  test_check(26U, (unsigned int)status.st_size == 300U);
  test_check(11U, access(first, F_OK) == 0);
  test_check(12U, rename(first, second) == 0);
  test_check(13U, access(second, F_OK) == 0);
  test_check(14U, unlink(second) == 0);

  errno = 0;
  test_check(15U, open(first, O_RDONLY) == -1 && errno == ENOENT);
}

static void test_directory_and_limits(void)
{
  struct stat status;
  unsigned int index;

  test_check(16U, getcwd(current_directory, sizeof(current_directory)) ==
                  current_directory && current_directory[0] != 0);
  test_check(17U, chdir(".") == 0);

  for (index = 0U; index < 255U; ++index)
    long_name[index] = 'x';
  long_name[255] = 0;

  errno = 0;
  test_check(18U, open(long_name, O_RDONLY) == -1 &&
                  errno == ENAMETOOLONG);
  errno = 0;
  test_check(19U, unlink(long_name) == -1 && errno == ENAMETOOLONG);
  errno = 0;
  test_check(20U, stat(long_name, &status) == -1 &&
                  errno == ENAMETOOLONG);
  errno = 0;
  test_check(21U, chdir(long_name) == -1 && errno == ENAMETOOLONG);
  errno = 0;
  test_check(22U, access(long_name, F_OK) == -1 &&
                  errno == ENAMETOOLONG);

  errno = 0;
  test_check(23U, fstat(0, &status) == -1 && errno == ENOENT);
  errno = 0;
  test_check(24U, lstat(".", &status) == -1 && errno == ENOENT);
}

void main(void)
{
  simulator_result = 0U;
  test_files();
  test_directory_and_limits();
  simulator_stop();
}
