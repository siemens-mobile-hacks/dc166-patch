#include "dc166_stdio.h"

char *gets(char *string)
{
  char *next = string;
  int character = fgetc(stdin);

  while (character != '\n' && character != EOF) {
    *next++ = (char)character;
    character = fgetc(stdin);
  }
  if (character == EOF)
    return 0;
  *next = 0;
  return string;
}
