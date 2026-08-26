#include "dc166_stdio.h"

char *fgets(char *string, int count, FILE *stream)
{
  char *next = string;
  int character = 0;

  if (count > 1) {
    character = fgetc(stream);
    if (character != EOF) {
      for (;;) {
        --count;
        *next++ = (char)character;
        if (character == '\n')
          break;
        if (count < 2)
          break;
        character = fgetc(stream);
        if (character == EOF)
          break;
      }
    }
  }
  if (character == EOF && next == string)
    return 0;
  *next = 0;
  return string;
}
