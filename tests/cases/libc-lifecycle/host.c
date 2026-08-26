#define DC166_TEST_NO_FP
#define DC166_TEST_NO_EXTENSIONS
#include "test.h"

typedef void signalfunction(int);

#define SIGINT 1
#define SIG_DFL ((signalfunction *)0)
#define SIG_IGN ((signalfunction *)1)
#define SIG_ERR ((signalfunction *)-1)

extern char *getenv(const char *);
extern int system(const char *);
extern int atexit(void (*)(void));
extern signalfunction *signal(int, signalfunction *);
extern int raise(int);

extern void _doexit(void);
extern int _simi(unsigned, char *, unsigned);
extern int _simo(unsigned, char *, unsigned);
extern void argcv_break(void);

static unsigned int callback_order;
static unsigned int callback_count;
static int handled_signal;

static void callback_one(void)
{
  callback_order = callback_order * 10U + 1U;
  ++callback_count;
}

static void callback_two(void)
{
  callback_order = callback_order * 10U + 2U;
  ++callback_count;
}

static void callback_three(void)
{
  callback_order = callback_order * 10U + 3U;
  ++callback_count;
}

static void callback_plain(void)
{
  ++callback_count;
}

static void signal_handler(int number)
{
  handled_signal = number;
}

void main(void)
{
  unsigned int index;
  int system_null;
  int system_text;
  char simio_buffer[1];
  unsigned int simio_sum;
  signalfunction *previous;

  simulator_result = 0U;
  callback_order = 0U;
  callback_count = 0U;
  handled_signal = 0;

  test_check(1U, getenv("PATH") == 0);
  system_null = system(0);
  system_text = system("anything");
  test_check(2U, system_null == 0 && system_text == 0);
  simio_sum = 1U + (unsigned int)simio_buffer + 3U;
  test_check(3U, _simi(1U, simio_buffer, 3U) == (int)simio_sum &&
                  _simo(1U, simio_buffer, 3U) == (int)simio_sum);
  argcv_break();

  test_check(4U, atexit(callback_one) == 0);
  test_check(5U, atexit(callback_two) == 0);
  test_check(6U, atexit(callback_three) == 0);
  _doexit();
  test_check(7U, callback_order == 321U && callback_count == 3U);

  for (index = 3U; index < 32U; ++index)
    test_check(8U, atexit(callback_plain) == 0);
  test_check(9U, atexit(callback_plain) == 1);

  previous = signal(SIGINT, signal_handler);
  test_check(10U, previous == SIG_DFL);
  test_check(11U, raise(SIGINT) == 0 && handled_signal == SIGINT);
  previous = signal(SIGINT, SIG_IGN);
  test_check(12U, previous == SIG_DFL);
  handled_signal = 0;
  test_check(13U, raise(SIGINT) == 0 && handled_signal == 0);
  test_check(14U, signal(0, signal_handler) == SIG_ERR);
  test_check(15U, signal(SIGINT, SIG_ERR) == SIG_ERR);
  test_check(16U, raise(0) == -1 && raise(7) == -1);

  simulator_stop();
}
