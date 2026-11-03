/*
 * raster - software drawing into a 32-bit ARGB framebuffer (level 3).
 *
 * Every drawing function takes a clip rectangle: nothing is drawn outside
 * of it or outside of the framebuffer.
 */
#ifndef CORE_RASTER_H
#define CORE_RASTER_H

#include "../std.h"
#include "../image/image.h"

enum {
    RAS_OK = 0,
    RAS_ERR_ARG,
    RAS_ERR_NOMEM
};

typedef struct RasRect {
    int x;
    int y;
    int w;
    int h;
} RasRect;

typedef struct RasFb {
    int width;
    int height;
    TkU32 *pixels;          /* 0xAARRGGBB, width * height */
} RasFb;

/* 8-bit alpha mask (e.g. a font atlas) and the region to draw from it. */
typedef struct RasMask {
    int stride;             /* row length of alpha, in bytes */
    RasRect src;
    const TkU8 *alpha;
} RasMask;

/* Image drawn with an integer zoom; lut maps indices to ARGB. */
typedef struct RasImage {
    int zoom;
    const Img *img;
    const TkU32 *lut;       /* 256 entries, indexed images only */
} RasImage;

/* ---- Framebuffer and rectangles (ras_base.c) ---- */

int ras_resize(RasFb *fb, int width, int height);
void ras_free(RasFb *fb);
RasRect ras_rect(
    int x,
    int y,
    int w,
    int h
);
RasRect ras_intersect(RasRect a, RasRect b);
void ras_fillRect(
    RasFb *fb,
    RasRect clip,
    RasRect rect,
    TkU32 argb
);

/* ---- Masks and images (ras_blit.c) ---- */

void ras_blitMask(
    RasFb *fb,
    RasRect clip,
    const RasMask *mask,
    int dstX,
    int dstY,
    TkU32 argb
);
void ras_blitImage(
    RasFb *fb,
    RasRect clip,
    const RasImage *image,
    int dstX,
    int dstY
);
void ras_buildLut(const Img *img, TkU32 *lut);

#endif /* CORE_RASTER_H */
