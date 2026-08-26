extern void _dc166_exit_target(void);
extern void cleanup(void);

void abort(void)
{
  cleanup();
  _dc166_exit_target();
  for (;;)
    ;
}
