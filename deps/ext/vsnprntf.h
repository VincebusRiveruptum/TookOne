#ifndef VSNPRNTF_H
#define VSNPRNTF_H

#if defined(__WATCOMC__) || defined(__MSDOS__)

#include <stdarg.h>
#include <stddef.h>

int vsnprintf(char *out, size_t size, const char *fmt, va_list ap);

#endif
#endif