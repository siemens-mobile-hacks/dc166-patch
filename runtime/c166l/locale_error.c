struct lconv {
  char *decimal_point;
  char *thousands_sep;
  char *grouping;
  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;
  char p_cs_precedes;
  char p_sep_by_space;
  char n_cs_precedes;
  char n_sep_by_space;
  char p_sign_posn;
  char n_sign_posn;
};

static struct lconv convention;

static int string_equal(const char *left, const char *right)
{
  while (*left != 0 && *left == *right) {
    ++left;
    ++right;
  }
  return *left == *right;
}

char *setlocale(int category, const char *locale)
{
  (void)category;
  if (locale == 0 || locale[0] == 0 || string_equal(locale, "C"))
    return (char *)"C";
  return 0;
}

struct lconv *localeconv(void)
{
  convention.decimal_point = (char *)".";
  convention.thousands_sep = (char *)"";
  convention.grouping = (char *)"";
  convention.int_curr_symbol = (char *)"";
  convention.currency_symbol = (char *)"";
  convention.mon_decimal_point = (char *)"";
  convention.mon_thousands_sep = (char *)"";
  convention.mon_grouping = (char *)"";
  convention.positive_sign = (char *)"";
  convention.negative_sign = (char *)"";
  convention.int_frac_digits = 127;
  convention.frac_digits = 127;
  convention.p_cs_precedes = 127;
  convention.p_sep_by_space = 127;
  convention.n_cs_precedes = 127;
  convention.n_sep_by_space = 127;
  convention.p_sign_posn = 127;
  convention.n_sign_posn = 127;
  return &convention;
}

char *strerror(int error)
{
  switch (error) {
  case 0: return "No error";
  case 1: return "Not owner";
  case 2: return "No such file or directory";
  case 3: return "Interrupted system call";
  case 4: return "I/O error";
  case 5: return "Bad file number";
  case 6: return "No more processes";
  case 7: return "Not enough core";
  case 8: return "Permission denied";
  case 9: return "Bad address";
  case 10: return "File exists";
  case 11: return "Not a directory";
  case 12: return "Is a directory";
  case 13: return "Invalid argument";
  case 14: return "File table overflow";
  case 15: return "Too many open files";
  case 16: return "Text file busy";
  case 17: return "No space left on device";
  case 18: return "Illegal seek";
  case 19: return "Read-only file system";
  case 20: return "Broken pipe";
  case 21: return "Too many levels of symbolic links";
  case 22: return "File name too long";
  case 23: return "Argument too large";
  case 24: return "Result too large";
  case 25: return "illegal format string for printf/scanf";
  case 26: return "floating point not supported";
  case 27: return "long not supported";
  case 28: return "pointers not supported";
  case 29: return "Positioning failure";
  case 30: return 0;
  default: return 0;
  }
}
