typedef void signal_function(int);

#define DC166_NSIG 7
#define DC166_SIG_DFL ((signal_function *)0)
#define DC166_SIG_IGN ((signal_function *)1)

extern signal_function *signaltable[DC166_NSIG];
extern void _doexit(void);
extern void _dc166_exit_target(void);

int raise(int number)
{
  signal_function *function;
  if (number <= 0 || number >= DC166_NSIG)
    return -1;
  function = signaltable[number];
  if (function == DC166_SIG_IGN)
    return 0;
  if (function != DC166_SIG_DFL) {
    signaltable[number] = DC166_SIG_DFL;
    function(number);
    return 0;
  }
  _doexit();
  _dc166_exit_target();
  for (;;)
    ;
}
