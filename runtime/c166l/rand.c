static unsigned long random_state = 1UL;

void srand(unsigned int seed)
{
  random_state = (unsigned long)seed;
}

int rand(void)
{
  random_state = random_state * 1103515245UL + 12345UL;
  return (int)((random_state >> 16) & 32767UL);
}
