extern void _doexit(void);
extern void _dc166_exit_target(void);

void exit(int status)
{
  (void)status;
  _doexit();
  _dc166_exit_target();
  for (;;)
    ;
}
