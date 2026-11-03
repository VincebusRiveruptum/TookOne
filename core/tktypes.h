/*
 * tktypes - fixed-size integer types for TookOne (C89, no <stdint.h>).
 */
#ifndef TKTYPES_H
#define TKTYPES_H

#include <limits.h>

typedef unsigned char TkU8;

#if USHRT_MAX == 0xFFFF
typedef unsigned short TkU16;
#else
#error "TookOne needs a 16-bit unsigned short"
#endif

#if UINT_MAX == 0xFFFFFFFFUL
typedef unsigned int TkU32;
#elif ULONG_MAX == 0xFFFFFFFFUL
typedef unsigned long TkU32;
#else
#error "TookOne needs a 32-bit unsigned type"
#endif

#endif /* TKTYPES_H */
