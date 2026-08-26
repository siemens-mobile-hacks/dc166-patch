typedef void exit_function(void);

static exit_function *exit_functions[32];
static unsigned char exit_function_count;

int atexit(exit_function *function)
{
  unsigned int index;

  if (exit_function_count == 32U)
    return 1;
  index = exit_function_count++;
  exit_functions[index] = function;
  return 0;
}

void _doexit(void)
{
  unsigned int index = exit_function_count;

  while (index != 0U)
    exit_functions[--index]();
}

char *getenv(const char *name)
{
  (void)name;
  return 0;
}
