typedef unsigned int wint_t;
typedef unsigned short wctype_t;
typedef unsigned short wctrans_t;

#define WEOF_VALUE 0xffffU
#define CLASS_ALNUM 0x001U
#define CLASS_ALPHA 0x002U
#define CLASS_CNTRL 0x004U
#define CLASS_DIGIT 0x008U
#define CLASS_GRAPH 0x010U
#define CLASS_LOWER 0x020U
#define CLASS_PRINT 0x040U
#define CLASS_PUNCT 0x080U
#define CLASS_SPACE 0x100U
#define CLASS_UPPER 0x200U
#define CLASS_XDIGIT 0x400U
#define CLASS_BLANK 0x800U
#define MAP_LOWER 0x1000U
#define MAP_UPPER 0x2000U

static int name_is(const char *name, const char *expected)
{
  while (*name != 0 && *name == *expected) {
    ++name;
    ++expected;
  }
  return *name == *expected;
}

static int ascii(wint_t value)
{
  return value != WEOF_VALUE && value <= 127U;
}

int iswctype(wint_t value, wctype_t descriptor)
{
  int upper;
  int lower;
  int digit;
  int space;
  int control;
  int punctuation;
  int result = 0;
  if (!ascii(value))
    return 0;
  upper = value >= 'A' && value <= 'Z';
  lower = value >= 'a' && value <= 'z';
  digit = value >= '0' && value <= '9';
  space = value == ' ' || (value >= '\t' && value <= '\r');
  control = value < 0x20U || value == 0x7fU;
  punctuation = value >= 0x21U && value <= 0x7eU &&
      !upper && !lower && !digit;
  if ((descriptor & CLASS_ALNUM) && (upper || lower || digit)) result = 1;
  if ((descriptor & CLASS_ALPHA) && (upper || lower)) result = 1;
  if ((descriptor & CLASS_CNTRL) && control) result = 1;
  if ((descriptor & CLASS_DIGIT) && digit) result = 1;
  if ((descriptor & CLASS_GRAPH) && value >= 0x21U && value <= 0x7eU) result = 1;
  if ((descriptor & CLASS_LOWER) && lower) result = 1;
  if ((descriptor & CLASS_PRINT) && value >= 0x20U && value <= 0x7eU) result = 1;
  if ((descriptor & CLASS_PUNCT) && punctuation) result = 1;
  if ((descriptor & CLASS_SPACE) && space) result = 1;
  if ((descriptor & CLASS_UPPER) && upper) result = 1;
  if ((descriptor & CLASS_XDIGIT) &&
      (digit || (value >= 'A' && value <= 'F') ||
       (value >= 'a' && value <= 'f'))) result = 1;
  if ((descriptor & CLASS_BLANK) && (value == ' ' || value == '\t')) result = 1;
  return result;
}

int iswalnum(wint_t value) { return iswctype(value, CLASS_ALNUM); }
int iswalpha(wint_t value) { return iswctype(value, CLASS_ALPHA); }
int iswcntrl(wint_t value) { return iswctype(value, CLASS_CNTRL); }
int iswdigit(wint_t value) { return iswctype(value, CLASS_DIGIT); }
int iswgraph(wint_t value) { return iswctype(value, CLASS_GRAPH); }
int iswlower(wint_t value) { return iswctype(value, CLASS_LOWER); }
int iswprint(wint_t value) { return iswctype(value, CLASS_PRINT); }
int iswpunct(wint_t value) { return iswctype(value, CLASS_PUNCT); }
int iswspace(wint_t value) { return iswctype(value, CLASS_SPACE); }
int iswupper(wint_t value) { return iswctype(value, CLASS_UPPER); }
int iswxdigit(wint_t value) { return iswctype(value, CLASS_XDIGIT); }

wctype_t wctype(const char *name)
{
  if (name_is(name, "alnum")) return CLASS_ALNUM;
  if (name_is(name, "alpha")) return CLASS_ALPHA;
  if (name_is(name, "cntrl")) return CLASS_CNTRL;
  if (name_is(name, "digit")) return CLASS_DIGIT;
  if (name_is(name, "graph")) return CLASS_GRAPH;
  if (name_is(name, "lower")) return CLASS_LOWER;
  if (name_is(name, "print")) return CLASS_PRINT;
  if (name_is(name, "punct")) return CLASS_PUNCT;
  if (name_is(name, "space")) return CLASS_SPACE;
  if (name_is(name, "upper")) return CLASS_UPPER;
  if (name_is(name, "xdigit")) return CLASS_XDIGIT;
  if (name_is(name, "blank")) return CLASS_BLANK;
  return 0U;
}

wctrans_t wctrans(const char *name)
{
  if (name_is(name, "tolower")) return MAP_LOWER;
  if (name_is(name, "toupper")) return MAP_UPPER;
  return 0U;
}

wint_t towctrans(wint_t value, wctrans_t descriptor)
{
  if (descriptor == MAP_LOWER && value >= 'A' && value <= 'Z')
    return value - 'A' + 'a';
  if (descriptor == MAP_UPPER && value >= 'a' && value <= 'z')
    return value - 'a' + 'A';
  return value;
}

wint_t towlower(wint_t value) { return towctrans(value, MAP_LOWER); }
wint_t towupper(wint_t value) { return towctrans(value, MAP_UPPER); }
