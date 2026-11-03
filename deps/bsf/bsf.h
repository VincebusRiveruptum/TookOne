/*
 * bsf - Bitmap Sequence File reader / writer.
 *
 * Generic dep: C89, standard library only, no cross-references.
 * Builds with gcc/clang/MSVC and OpenWatcom (DOS/4G).
 * Shared by TookOne (editor) and BMPIO (game engine).
 *
 * Format specification: docs/bsf_format.md
 */
#ifndef BSF_H
#define BSF_H

#include <stdio.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Fixed-size integer types (no <stdint.h> in Watcom 10.6) ---- */

typedef unsigned char BsfU8;

#if USHRT_MAX == 0xFFFF
typedef unsigned short BsfU16;
#else
#error "bsf needs a 16-bit unsigned short"
#endif

#if UINT_MAX == 0xFFFFFFFFUL
typedef unsigned int BsfU32;
#elif ULONG_MAX == 0xFFFFFFFFUL
typedef unsigned long BsfU32;
#else
#error "bsf needs a 32-bit unsigned type"
#endif

/* ---- Constants ---- */

#define BSF_VERSION         1
#define BSF_MAX_DIM         4096    /* max width / height in pixels */
#define BSF_NAME_LEN        63      /* useful chars; longer names truncated */
#define BSF_PALETTE_MAX     256

/* BsfSequence.flags */
enum {
    BSF_FLAG_TRANSPARENT = 0x01,
    BSF_FLAG_LOOP        = 0x02
};

/* Frame compression */
enum {
    BSF_COMP_RAW     = 0,
    BSF_COMP_RLE     = 1,
    BSF_COMP_DEFLATE = 2    /* reserved, not supported yet */
};

/* bsfRead options */
enum {
    BSF_READ_NO_CRC = 0x01  /* skip CRC checks (faster loading in games) */
};

/* Error codes */
enum {
    BSF_OK = 0,
    BSF_ERR_IO,             /* read/write failed or unexpected EOF */
    BSF_ERR_NOMEM,          /* allocation failed */
    BSF_ERR_SIGNATURE,      /* not a .bsf file */
    BSF_ERR_VERSION,        /* written by a newer version */
    BSF_ERR_FORMAT,         /* malformed or inconsistent data */
    BSF_ERR_CRC,            /* chunk checksum mismatch */
    BSF_ERR_UNSUPPORTED,    /* valid but unsupported (e.g. deflate) */
    BSF_ERR_ARG             /* invalid argument */
};

/* ---- Sequence model ----
 *
 * Pixels are stored packed exactly as in the file: rows top to bottom,
 * no row padding, bsfRowBytes() bytes per row.
 *   1 bpp: palette indices, MSB first   4 bpp: indices, high nibble first
 *   8 bpp: one index per byte           16 bpp: RGB565 little-endian
 *  24 bpp: R, G, B
 */

typedef struct BsfFrame {
    BsfU16 delay;           /* ms; 0 = use BsfSequence.defaultDelay */
    BsfU8 *pixels;          /* bsfFrameBytes() bytes */
} BsfFrame;

typedef struct BsfSequence {
    BsfU16 width;
    BsfU16 height;
    BsfU8 bpp;              /* 1, 4, 8, 16 or 24 */
    BsfU8 flags;            /* BSF_FLAG_* */
    BsfU32 transparent;     /* palette index (bpp <= 8) or 0x00RRGGBB */
    BsfU16 defaultDelay;    /* ms */
    BsfU16 loopCount;       /* 0 = infinite (with BSF_FLAG_LOOP) */

    BsfU16 paletteCount;    /* required (>= 1) when bpp <= 8 */
    BsfU8 palette[BSF_PALETTE_MAX * 3];     /* R, G, B */

    char name[BSF_NAME_LEN + 1];            /* UTF-8, "" = none */

    BsfU16 frameCount;
    BsfFrame *frames;
} BsfSequence;

const char *bsfStrError(int err);

BsfU32 bsfRowBytes(BsfU16 width, BsfU8 bpp);
BsfU32 bsfFrameBytes(const BsfSequence *seq);

/*
 * Zeroes *seq, sets the format and allocates frameCount zero-filled frames.
 * The palette is left empty. Free with bsfFree().
 */
int bsfInit(
    BsfSequence *seq,
    BsfU16 width,
    BsfU16 height,
    BsfU8 bpp,
    BsfU16 frameCount
);
void bsfFree(BsfSequence *seq);

/* Checks that the sequence can be written as a valid .bsf. */
int bsfValidate(const BsfSequence *seq);

/*
 * Both work from the current file position, so a .bsf embedded in a larger
 * file (.bss, .bpk) can be read after an fseek(). Open files in binary mode.
 */

/* On success *seq owns its memory (bsfFree). On error *seq is left empty. */
int bsfRead(FILE *fp, BsfSequence *seq, unsigned int options);

/*
 * compression: BSF_COMP_RAW or BSF_COMP_RLE. With RLE, frames that would
 * not shrink are stored raw.
 */
int bsfWrite(FILE *fp, const BsfSequence *seq, int compression);

#ifdef __cplusplus
}
#endif

#endif /* BSF_H */
