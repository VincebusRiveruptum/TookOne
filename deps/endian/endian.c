/*
 * endian - little-endian integer reading from byte buffers.
 */
#include "endian.h"

/* ================================================================
 * 1. Public API
 * ================================================================ */

/*
 * Reads an unsigned 16-bit little-endian value.
 * Expects: p pointing to 2 readable bytes.
 * Returns: the value, 0..65535.
 */
unsigned long endianReadU16(const unsigned char *p)
{
    return (unsigned long)p[0] | ((unsigned long)p[1] << 8);
}

/*
 * Reads an unsigned 32-bit little-endian value.
 * Expects: p pointing to 4 readable bytes.
 * Returns: the value, 0..0xFFFFFFFF.
 */
unsigned long endianReadU32(const unsigned char *p)
{
    return (unsigned long)p[0]
        | ((unsigned long)p[1] << 8)
        | ((unsigned long)p[2] << 16)
        | ((unsigned long)p[3] << 24);
}

/*
 * Reads a signed (two's complement) 32-bit little-endian value.
 * Expects: p pointing to 4 readable bytes.
 * Returns: the value, -2147483648..2147483647.
 */
long endianReadS32(const unsigned char *p)
{
    unsigned long v;

    v = endianReadU32(p);
    if (v & 0x80000000UL)
        return -(long)(~v & 0x7FFFFFFFUL) - 1L;
    return (long)v;
}

#ifdef STANDALONE
/* ================================================================
 * 4. Tests: built with -DSTANDALONE as a test program
 * ================================================================ */
#include <stdio.h>

static int _failures = 0;
static int _checks = 0;

#define ENDIAN_CHECK(cond) do { \
    _checks++; \
    if (!(cond)) { \
        _failures++; \
        printf( \
            "FAIL %s:%d: %s\n", \
            __FILE__, \
            __LINE__, \
            #cond \
        ); \
    } \
} while (0)

/*
 * Runs the endian tests.
 * Expects: nothing.
 * Returns: 0 when every check passes, 1 otherwise.
 */
int main(void)
{
    static const unsigned char b[4] = { 0x78, 0x56, 0x34, 0x12 };
    static const unsigned char neg[4] = { 0xFE, 0xFF, 0xFF, 0xFF };
    static const unsigned char min[4] = { 0x00, 0x00, 0x00, 0x80 };
    static const unsigned char max[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

    ENDIAN_CHECK(endianReadU16(b) == 0x5678UL);
    ENDIAN_CHECK(endianReadU32(b) == 0x12345678UL);
    ENDIAN_CHECK(endianReadU32(max) == 0xFFFFFFFFUL);
    ENDIAN_CHECK(endianReadS32(b) == 0x12345678L);
    ENDIAN_CHECK(endianReadS32(neg) == -2L);
    ENDIAN_CHECK(endianReadS32(max) == -1L);
    ENDIAN_CHECK(endianReadS32(min) == -2147483647L - 1L);

    printf("endian: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}

#endif /* STANDALONE */
