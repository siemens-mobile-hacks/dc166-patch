#include "test.h"

typedef unsigned int size_t;

extern char _far *_fstrcpy(char _far *, const char _far *);
extern int _fstrcmp(const char _far *, const char _far *);
extern char _far *_fstrtok(char _far *, const char _far *);
extern char _huge *_hstrcpy(char _huge *, const char _huge *);
extern int _hstrcmp(const char _huge *, const char _huge *);
extern char _huge *_hstrtok(char _huge *, const char _huge *);
extern char _shuge *_sstrcpy(char _shuge *, const char _shuge *);
extern int _sstrcmp(const char _shuge *, const char _shuge *);
extern char _shuge *_sstrtok(char _shuge *, const char _shuge *);

static char _far far_text[24];
static char _far far_delimiters[3];
static char _huge huge_text[24];
static char _huge huge_delimiters[3];
static char _shuge shuge_text[24];
static char _shuge shuge_delimiters[3];

static void test_far(void)
{
  char _far *token;

  _fstrcpy(far_text, (const char _far *)"one,two;three");
  _fstrcpy(far_delimiters, (const char _far *)",;");
  token = _fstrtok(far_text, far_delimiters);
  test_check(1U, token != 0 &&
                 _fstrcmp(token, (const char _far *)"one") == 0);
  token = _fstrtok(0, far_delimiters);
  test_check(2U, token != 0 &&
                 _fstrcmp(token, (const char _far *)"two") == 0);
  token = _fstrtok(0, far_delimiters);
  test_check(3U, token != 0 &&
                 _fstrcmp(token, (const char _far *)"three") == 0);
  test_check(4U, _fstrtok(0, far_delimiters) == 0);
}

static void test_huge(void)
{
  char _huge *token;

  _hstrcpy(huge_text, (const char _huge *)"one,two;three");
  _hstrcpy(huge_delimiters, (const char _huge *)",;");
  token = _hstrtok(huge_text, huge_delimiters);
  test_check(5U, token != 0 &&
                 _hstrcmp(token, (const char _huge *)"one") == 0);
  token = _hstrtok(0, huge_delimiters);
  test_check(6U, token != 0 &&
                 _hstrcmp(token, (const char _huge *)"two") == 0);
  token = _hstrtok(0, huge_delimiters);
  test_check(7U, token != 0 &&
                 _hstrcmp(token, (const char _huge *)"three") == 0);
  test_check(8U, _hstrtok(0, huge_delimiters) == 0);
}

static void test_shuge(void)
{
  char _shuge *token;

  _sstrcpy(shuge_text, (const char _shuge *)"one,two;three");
  _sstrcpy(shuge_delimiters, (const char _shuge *)",;");
  token = _sstrtok(shuge_text, shuge_delimiters);
  test_check(9U, token != 0 &&
                 _sstrcmp(token, (const char _shuge *)"one") == 0);
  token = _sstrtok(0, shuge_delimiters);
  test_check(10U, token != 0 &&
                  _sstrcmp(token, (const char _shuge *)"two") == 0);
  token = _sstrtok(0, shuge_delimiters);
  test_check(11U, token != 0 &&
                  _sstrcmp(token, (const char _shuge *)"three") == 0);
  test_check(12U, _sstrtok(0, shuge_delimiters) == 0);
}

void main(void)
{
  simulator_result = 0U;
  test_far();
  test_huge();
  test_shuge();
  simulator_stop();
}
