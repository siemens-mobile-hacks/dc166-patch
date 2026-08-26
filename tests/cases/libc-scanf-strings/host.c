#include "test.h"
#include <stdio.h>
#include <string.h>


static char text[96];
static int default_anchor;
static int _near near_anchor;

static void test_strings_and_characters(void)
{
  char word[12];
  char characters[4] = { 0, 0, 0, 0 };
  char tail = 0;
  int count = -1;
  int result;

  result = sscanf("  alpha XYZ!", "%s %3c%c%n", word, characters,
                  &tail, &count);
  test_check(1U, result == 3 && strcmp(word, "alpha") == 0 &&
                 memcmp(characters, "XYZ", 3U) == 0 && tail == '!' &&
                 count == 12);

  memset(word, 0, sizeof(word));
  tail = 0;
  result = sscanf("skip keep!", "%*s %4s%c", word, &tail);
  test_check(2U, result == 2 && strcmp(word, "keep") == 0 && tail == '!');

  tail = 0;
  result = sscanf("%Q", "%%%c", &tail);
  test_check(3U, result == 1 && tail == 'Q');
}

static void test_scansets(void)
{
  char selected[12];
  char rest[12];
  int result;

  result = sscanf("abcaXYZ", "%[abc]%[^!]", selected, rest);
  test_check(4U, result == 2 && strcmp(selected, "abca") == 0 &&
                 strcmp(rest, "XYZ") == 0);

  result = sscanf("XYZ", "%[abc]", selected);
  test_check(5U, result == 0);

  result = sscanf("]aaX", "%[]a]%c", selected, rest);
  test_check(6U, result == 2 && strcmp(selected, "]aa") == 0 &&
                 rest[0] == 'X');
}

static void test_pointers(void)
{
  void *default_value = 0;
  void _near *near_source = (void _near *)&near_anchor;
  void _near *near_value = 0;
#if _MODEL != 't'
  void _far *far_value = 0;
  void _huge *huge_value = 0;
#endif
  int result;

  sprintf(text, "%p", (void *)&default_anchor);
  result = sscanf(text, "%p", &default_value);
  test_check(7U, result == 1 && default_value == (void *)&default_anchor);

  sprintf(text, "%hp", near_source);
  result = sscanf(text, "%hp", &near_value);
  test_check(80U, result == 1);
  test_check(81U, near_value == near_source);

#if _MODEL != 't'
  sprintf(text, "%lp", (void _far *)&default_anchor);
  result = sscanf(text, "%lp", &far_value);
  test_check(9U, result == 1 && far_value == (void _far *)&default_anchor);

  sprintf(text, "%Lp", (void _huge *)&default_anchor);
  result = sscanf(text, "%Lp", &huge_value);
  test_check(10U, result == 1 && huge_value == (void _huge *)&default_anchor);
#endif
}

void main(void)
{
  simulator_result = 0U;
  test_strings_and_characters();
  if (simulator_result != 0U)
    simulator_stop();
  test_scansets();
  if (simulator_result != 0U)
    simulator_stop();
  test_pointers();
  simulator_stop();
}
