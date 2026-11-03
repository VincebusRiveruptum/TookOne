/*
 * image - in-memory image and its file codecs (level 2).
 *
 * Indexed images (1/4/8 bpp) keep one palette index per byte in memory,
 * whatever the file depth is; packing is the job of the codecs.
 * Direct color images (16/24 bpp) keep one 0x00RRGGBB value per pixel.
 */
#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "../std.h"

#define IMG_MAX_DIM         8192
#define IMG_PALETTE_MAX     256

enum {
    IMG_OK = 0,
    IMG_ERR_ARG,            /* invalid argument */
    IMG_ERR_NOMEM,          /* allocation failed */
    IMG_ERR_IO,             /* cannot open / read the file */
    IMG_ERR_SIGNATURE,      /* not a file of the expected format */
    IMG_ERR_FORMAT,         /* malformed header or data */
    IMG_ERR_UNSUPPORTED     /* valid file, unsupported variant (RLE...) */
};

typedef struct Img {
    int width;
    int height;
    int bpp;                            /* 1, 4, 8, 16 or 24 */
    int paletteCount;
    TkU32 palette[IMG_PALETTE_MAX];     /* 0x00RRGGBB */
    TkU8 *index;                        /* bpp <= 8: width * height */
    TkU32 *rgb;                         /* bpp > 8:  width * height */
} Img;

/* ---- Lifecycle (img_base.c) ---- */

int img_create(
    Img *img,
    int width,
    int height,
    int bpp
);
void img_free(Img *img);
int img_isIndexed(const Img *img);
const char *img_strError(int err);

/* ---- BMP codec (img_bmp.c) ----
 * Uncompressed 1, 4, 8, 16 (555 / 565 bitfields), 24 and 32 bpp,
 * bottom-up and top-down, BITMAPINFOHEADER and V4 / V5 headers.
 * 32 bpp images are loaded as 24 bpp (alpha is dropped).
 */

int img_loadBmp(const char *path, Img *img);

#endif /* CORE_IMAGE_H */
