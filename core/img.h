/*
 * img - in-memory image: indexed (1/4/8 bpp) or direct color (16/24 bpp).
 *
 * Indexed images keep one palette index per byte in memory, whatever the
 * file depth is; packing is the job of the codecs.
 * Direct color images keep one 0x00RRGGBB value per pixel.
 */
#ifndef IMG_H
#define IMG_H

#include "tktypes.h"

#define IMG_MAX_DIM         8192
#define IMG_PALETTE_MAX     256

enum {
    IMG_OK = 0,
    IMG_ERR_ARG,
    IMG_ERR_NOMEM
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

/* Zeroes *img and allocates zero-filled pixels. */
int img_create(
    Img *img,
    int width,
    int height,
    int bpp
);
void img_free(Img *img);

int img_isIndexed(const Img *img);

#endif /* IMG_H */
