/*
 * bmp - Windows BMP reader.
 *
 * Supports uncompressed 1, 4, 8, 16 (555 / 565 bitfields), 24 and 32 bpp,
 * bottom-up and top-down, BITMAPINFOHEADER and newer (V4 / V5) headers.
 * 32 bpp images are loaded as 24 bpp (alpha is dropped).
 */
#ifndef BMP_H
#define BMP_H

#include "img.h"

enum {
    BMP_OK = 0,
    BMP_ERR_IO,             /* cannot open / read the file */
    BMP_ERR_NOMEM,
    BMP_ERR_SIGNATURE,      /* not a BMP file */
    BMP_ERR_FORMAT,         /* malformed header or data */
    BMP_ERR_UNSUPPORTED     /* valid BMP, unsupported variant (RLE, ...) */
};

const char *bmp_strError(int err);

/* On success *img owns its memory (img_free). On error *img is empty. */
int bmp_load(const char *path, Img *img);

#endif /* BMP_H */
