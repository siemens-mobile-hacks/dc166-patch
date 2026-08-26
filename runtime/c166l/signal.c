typedef void signal_function(int);

#define DC166_NSIG 7
#define DC166_SIG_DFL ((signal_function *)0)
#define DC166_SIG_IGN ((signal_function *)1)
#define DC166_SIG_ERR ((signal_function *)-1)

signal_function *signaltable[DC166_NSIG];

signal_function *signal(int number, signal_function *function)
{
  signal_function *previous;

  if (number <= 0 || number >= DC166_NSIG || function == DC166_SIG_ERR)
    return DC166_SIG_ERR;
  previous = signaltable[number];
  signaltable[number] = function;
  return previous;
}
