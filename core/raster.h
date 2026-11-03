/*
 * raster - software drawing into a 32-bit ARGB framebuffer.
 *
 * Every function takes a clip rectangle; nothing is drawn outside of it
 * or outside of the framebuffer.
 */
#ifndef RASTER_H
#define RASTER_H

#include "tktypes.h"
#include "img.h"

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

enum {
    RAS_OK = 0,
    RAS_ERR_ARG,
    RAS_ERR_NOMEM
};

/* (Re)allocates the framebuffer; keeps *fb unchanged on failure. */
int ras_resize(RasFb *fb, int width, int height);
void ras_free(RasFb *fb);

RasRect ras_rect(
    int x,
    int y,
    int w,
    int h
);
RasRect ras_intersect(RasRect a, RasRect b);

/* Fills rect with an ARGB color; alpha < 255 blends. */
void ras_fillRect(
    RasFb *fb,
    RasRect clip,
    RasRect rect,
    TkU32 argb
);

/*
 * Tints an 8-bit alpha mask (e.g. a font atlas) with argb and blends it.
 * src is the region inside the mask, maskStride its row length in bytes.
 */
typedef struct RasMask {
    const TkU8 *alpha;
    int stride;
    RasRect src;
} RasMask;

void ras_blitMask(
    RasFb *fb,
    RasRect clip,
    const RasMask *mask,
    int dstX,
    int dstY,
    TkU32 argb
);

/*
 * Draws img scaled by an integer zoom (nearest neighbor) with its top-left
 * corner at (dstX, dstY). lut maps palette indices to ARGB (indexed images
 * only, 256 entries).
 */
typedef struct RasImage {
    const Img *img;
    const TkU32 *lut;
    int zoom;
} RasImage;

void ras_blitImage(
    RasFb *fb,
    RasRect clip,
    const RasImage *image,
    int dstX,
    int dstY
);

/* Builds the 256-entry ARGB lookup table for an indexed image. */
void ras_buildLut(const Img *img, TkU32 *lut);

#endif /* RASTER_H */
