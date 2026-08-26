#ifndef DC166_STDIO_H
#define DC166_STDIO_H

#include <stdio.h>
#include <wchar.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* stdio.h exposes these as macros; the library must also export functions. */
#undef getc
#undef getchar
#undef putc
#undef putchar
#undef clearerr
#undef feof
#undef ferror

extern int _close(int);
extern long _lseek(int, long, int);
extern unsigned int _read(int, char *, unsigned int);
extern unsigned int _write(int, char *, unsigned int);
extern int _unlink(const char *);
extern int _doprint(FILE *, const char *, va_list);
extern int _doscan(FILE *, const char *, va_list);
extern int _dowprint(FILE *, const wchar_t *, va_list);
extern int _dowscan(FILE *, const wchar_t *, va_list);
extern wint_t _wfilbuf(FILE *);
extern wint_t _wflsbuf(wchar_t, FILE *);
typedef void dc166_free_function(void *);
extern dc166_free_function *_ptr_to_free;

#define DC166_STREAM_OPEN(fp) (((fp)->_flag & (_IORD | _IOWR)) != 0U)
#define DC166_STREAM_WIDE(fp) (((fp)->_flag & _IOWCHAR) != 0U)

#endif
