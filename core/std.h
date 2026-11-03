/*
 * std - definitions shared by the whole project (level 1).
 *
 * Common standard includes and fixed-size integer types (C89 has no
 * <stdint.h>, and Watcom 10.6 does not provide it).
 */
#ifndef CORE_STD_H
#define CORE_STD_H

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#endif /* CORE_STD_H */
