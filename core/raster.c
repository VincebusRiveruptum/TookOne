#include <stdlib.h>
#include <string.h>
#include "raster.h"

int ras_resize(RasFb *fb, int width, int height)
{
    TkU32 *pixels = NULL;

    if (!fb || width < 1 || height < 1)
        return RAS_ERR_ARG;
    if (fb->pixels && fb->width == width && fb->height == height)
        return RAS_OK;

    pixels = (TkU32 *)malloc((size_t)width * (size_t)height * sizeof(TkU32));
    if (!pixels)
        return RAS_ERR_NOMEM;
    free(fb->pixels);
    fb->pixels = pixels;
    fb->width = width;
    fb->height = height;
    return RAS_OK;
}

void ras_free(RasFb *fb)
{
    if (!fb)
        return;
    free(fb->pixels);
    memset(fb, 0, sizeof(*fb));
}

RasRect ras_rect(
    int x,
    int y,
    int w,
    int h
)
{
    RasRect r;

    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    return r;
}

RasRect ras_intersect(RasRect a, RasRect b)
{
    int x0;
    int y0;
    int x1;
    int y1;

    x0 = a.x > b.x ? a.x : b.x;
    y0 = a.y > b.y ? a.y : b.y;
    x1 = a.x + a.w < b.x + b.w ? a.x + a.w : b.x + b.w;
    y1 = a.y + a.h < b.y + b.h ? a.y + a.h : b.y + b.h;
    if (x1 < x0)
        x1 = x0;
    if (y1 < y0)
        y1 = y0;
    return ras_rect(
        x0,
        y0,
        x1 - x0,
        y1 - y0
    );
}

/* Visible area: rect ∩ clip ∩ framebuffer. */
static RasRect _visible(const RasFb *fb, RasRect clip, RasRect rect)
{
    RasRect r;

    r = ras_intersect(rect, clip);
    return ras_intersect(r, ras_rect(
        0,
        0,
        fb->width,
        fb->height
    ));
}

/* Blends src over dst with alpha a (0..255). */
static TkU32 _blend(TkU32 dst, TkU32 src, TkU32 a)
{
    TkU32 r;
    TkU32 g;
    TkU32 b;
    TkU32 ia;

    if (a >= 255)
        return 0xFF000000UL | (src & 0x00FFFFFFUL);
    if (a == 0)
        return dst;
    ia = 255 - a;
    r = (((src >> 16) & 0xFF) * a + ((dst >> 16) & 0xFF) * ia) / 255;
    g = (((src >> 8) & 0xFF) * a + ((dst >> 8) & 0xFF) * ia) / 255;
    b = ((src & 0xFF) * a + (dst & 0xFF) * ia) / 255;
    return 0xFF000000UL | (r << 16) | (g << 8) | b;
}

void ras_fillRect(
    RasFb *fb,
    RasRect clip,
    RasRect rect,
    TkU32 argb
)
{
    int x;
    int y;
    TkU32 a;
    RasRect r;
    TkU32 *row = NULL;

    r = _visible(fb, clip, rect);
    a = (argb >> 24) & 0xFF;
    for (y = r.y; y < r.y + r.h; y++) {
        row = fb->pixels + (size_t)y * (size_t)fb->width;
        for (x = r.x; x < r.x + r.w; x++)
            row[x] = _blend(row[x], argb, a);
    }
}

void ras_blitMask(
    RasFb *fb,
    RasRect clip,
    const RasMask *mask,
    int dstX,
    int dstY,
    TkU32 argb
)
{
    int x;
    int y;
    int sx;
    int sy;
    TkU32 a;
    TkU32 colorA;
    RasRect r;
    TkU32 *row = NULL;

    r = _visible(fb, clip, ras_rect(
        dstX,
        dstY,
        mask->src.w,
        mask->src.h
    ));
    colorA = (argb >> 24) & 0xFF;
    for (y = r.y; y < r.y + r.h; y++) {
        row = fb->pixels + (size_t)y * (size_t)fb->width;
        sy = mask->src.y + (y - dstY);
        for (x = r.x; x < r.x + r.w; x++) {
            sx = mask->src.x + (x - dstX);
            a = mask->alpha[(size_t)sy * (size_t)mask->stride + (size_t)sx];
            a = a * colorA / 255;
            row[x] = _blend(row[x], argb, a);
        }
    }
}

void ras_blitImage(
    RasFb *fb,
    RasRect clip,
    const RasImage *image,
    int dstX,
    int dstY
)
{
    int x;
    int y;
    int sx;
    int sy;
    int zoom;
    size_t src;
    RasRect r;
    TkU32 *row = NULL;
    const Img *img = NULL;

    img = image->img;
    zoom = image->zoom < 1 ? 1 : image->zoom;
    r = ras_rect(
        dstX,
        dstY,
        img->width * zoom,
        img->height * zoom
    );
    r = _visible(fb, clip, r);

    for (y = r.y; y < r.y + r.h; y++) {
        row = fb->pixels + (size_t)y * (size_t)fb->width;
        sy = (y - dstY) / zoom;
        for (x = r.x; x < r.x + r.w; x++) {
            sx = (x - dstX) / zoom;
            src = (size_t)sy * (size_t)img->width + (size_t)sx;
            if (img_isIndexed(img))
                row[x] = image->lut[img->index[src]];
            else
                row[x] = 0xFF000000UL | img->rgb[src];
        }
    }
}

void ras_buildLut(const Img *img, TkU32 *lut)
{
    int i;

    for (i = 0; i < IMG_PALETTE_MAX; i++) {
        if (i < img->paletteCount)
            lut[i] = 0xFF000000UL | img->palette[i];
        else
            lut[i] = 0xFF000000UL;
    }
}
