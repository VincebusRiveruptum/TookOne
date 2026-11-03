/*
 * image/private.h - internals of the image module.
 * Included only by the module's own .c files.
 */
#ifndef CORE_IMAGE_PRIVATE_H
#define CORE_IMAGE_PRIVATE_H

#include "image.h"

/* ---- BMP file layout ---- */

#define IMG_BMP_FILE_HDR        14
#define IMG_BMP_INFO_MIN        40
#define IMG_BMP_MASKS_AT        54  /* bitfield masks, any header >= 40 */
#define IMG_BMP_MAX_FILE        (512L * 1024L * 1024L)

#define IMG_BMP_BI_RGB          0
#define IMG_BMP_BI_BITFIELDS    3
#define IMG_BMP_BI_ALPHAFIELDS  6

/* Channel extraction for 16 / 32 bpp. */
typedef struct ImgBmpMask {
    int shift;
    TkU32 mask;
    TkU32 max;
} ImgBmpMask;

/* Parsed BMP header. */
typedef struct ImgBmpInfo {
    int width;
    int height;
    int topDown;
    int bpp;
    long rowSize;
    TkU32 dataOffset;
    TkU32 paletteOffset;
    TkU32 paletteCount;
    ImgBmpMask masks[3];                /* R, G, B */
} ImgBmpInfo;

#endif /* CORE_IMAGE_PRIVATE_H */
