/*
 * endian - little-endian integer reading from byte buffers.
 *
 * Generic dep: C89, standard library only, no cross-references.
 * Results are exact on any host byte order and any `long` size.
 */
#ifndef DEPS_ENDIAN_H
#define DEPS_ENDIAN_H

/* ---- Reading (p points to at least 2 / 4 bytes) ---- */

unsigned long endianReadU16(const unsigned char *p);
unsigned long endianReadU32(const unsigned char *p);
long endianReadS32(const unsigned char *p);

#endif /* DEPS_ENDIAN_H */
