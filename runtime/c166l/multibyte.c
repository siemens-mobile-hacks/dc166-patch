typedef unsigned int size_t;
typedef int wchar_t;

typedef struct {
  wchar_t wc_value;
  unsigned short n_bytes;
  unsigned short encoding;
} mbstate_t;

extern int errno;

int mbtowc(wchar_t *wide, const char *source, size_t count);

static mbstate_t mbrtowc_state;
static mbstate_t wcrtomb_state;

static void reset_state(mbstate_t *state)
{
  if (state != 0) {
    state->wc_value = 0;
    state->n_bytes = 0U;
    state->encoding = 0U;
  }
}

int mbsinit(const mbstate_t *state)
{
  return state == 0 ||
      (state->wc_value == 0 && state->n_bytes == 0U && state->encoding == 0U);
}

size_t mbrtowc(wchar_t *wide, const char *source, size_t count,
               mbstate_t *state)
{
  size_t solved_before = 0U;
  size_t solved;
  int complete = 0;

  /* This is the exact reset shortcut used by the TASKING Small library. */
  if (source == 0 ||
      (wide == 0 && source[0] == 0 && count == 1U)) {
    reset_state(&mbrtowc_state);
    return 0U;
  }

  if (state == 0)
    state = &mbrtowc_state;

  solved = state->n_bytes;
  while (solved < count) {
    if (state->encoding == 0U) {
      state->wc_value = (wchar_t)(signed char)source[0];
      state->n_bytes = 1U;
      complete = 1;
      break;
    }
    if (!mbsinit(state))
      solved_before = state->n_bytes;
    ++solved;
  }

  if (complete) {
    size_t result = state->wc_value == 0 ? 0U :
        (size_t)(state->n_bytes - solved_before);
    if (wide != 0)
      *wide = state->wc_value;
    reset_state(state);
    return result;
  }
  if (solved == count)
    return (size_t)-2;

  errno = 30;
  reset_state(&mbrtowc_state);
  return (size_t)-1;
}

size_t mbrlen(const char *source, size_t count, mbstate_t *state)
{
  mbstate_t local_state;
  if (state == 0) {
    reset_state(&local_state);
    state = &local_state;
  }
  return mbrtowc(0, source, count, state);
}

size_t wcrtomb(char *destination, wchar_t wide, mbstate_t *state)
{
  char local_destination;

  if (state == 0)
    state = &wcrtomb_state;
  if (destination == 0) {
    destination = &local_destination;
    wide = 0;
  }
  if (wide < 0 || wide > 255) {
    errno = 30;
    reset_state(&wcrtomb_state);
    return (size_t)-1;
  }
  *destination = (char)wide;
  reset_state(state);
  return 1U;
}

int mblen(const char *source, size_t count)
{
  return mbtowc(0, source, count);
}

int mbtowc(wchar_t *wide, const char *source, size_t count)
{
  signed char character;

  if (source == 0 || count == 0U)
    return 0;
  character = (signed char)source[0];
  if (wide != 0)
    *wide = (wchar_t)character;
  return character == 0 ? 0 : 1;
}

int wctomb(char *destination, wchar_t wide)
{
  if (destination == 0)
    return 0;
  *destination = (char)wide;
  return 1;
}

size_t mbsrtowcs(wchar_t *destination, const char **source,
                 size_t count, mbstate_t *state)
{
  mbstate_t local_state;
  const char *input;
  size_t converted = 0U;
  wchar_t wide = 0;

  if (source == 0 || *source == 0)
    return 0U;
  if (state == 0)
    reset_state(&local_state);
  else
    local_state = *state;
  input = *source;

  while (converted < count) {
    size_t result = mbrtowc(&wide, input, 1U, &local_state);
    if (result == (size_t)-1 || result == (size_t)-2)
      return (size_t)-1;
    if (destination != 0)
      destination[converted] = wide;
    if (wide == 0)
      break;
    ++converted;
    ++input;
  }

  if (wide == 0)
    *source = 0;
  else
    *source = input;
  if (state != 0)
    *state = local_state;
  return converted;
}

size_t wcsrtombs(char *destination, const wchar_t **source,
                 size_t count, mbstate_t *state)
{
  mbstate_t local_state;
  const wchar_t *input;
  size_t converted = 0U;

  if (source == 0 || *source == 0)
    return 0U;
  if (state == 0)
    reset_state(&local_state);
  else
    local_state = *state;
  input = *source;

  while (converted < count) {
    char byte;
    if (wcrtomb(&byte, *input, &local_state) == (size_t)-1)
      return (size_t)-1;
    if (destination != 0)
      destination[converted] = byte;
    if (*input == 0) {
      input = 0;
      break;
    }
    ++converted;
    ++input;
  }

  *source = input;
  if (state != 0)
    *state = local_state;
  return converted;
}

size_t mbstowcs(wchar_t *destination, const char *source, size_t count)
{
  size_t converted = 0U;

  while (converted < count) {
    int result = mbtowc(destination, source, 1U);
    if (result == 0)
      break;
    ++converted;
    ++source;
    if (destination != 0)
      ++destination;
  }
  return converted;
}

size_t wcstombs(char *destination, const wchar_t *source, size_t count)
{
  size_t converted = 0U;

  while (converted < count) {
    if (wctomb(destination, *source) == 0)
      break;
    if (*source == 0)
      break;
    ++converted;
    ++source;
    if (destination != 0)
      ++destination;
  }
  return converted;
}
