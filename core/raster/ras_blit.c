/*
 * ras_blit - alpha masks and images (module: raster).
 */
#include "private.h"

/* ================================================================
 * 1. Public API
 * ================================================================ */

/*
 * Tints an 8-bit alpha mask with a color and blends it.
 * Expects: a framebuffer, the clip, the mask, its top-left destination
 *          and a 0xAARRGGBB color.
 * Returns: nothing.
 */
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

    r = mask->src;
    r.x = dstX;
    r.y = dstY;
    r = _ras_visible(fb, clip, r);
    colorA = (argb >> 24) & 0xFF;
    for (y = r.y; y < r.y + r.h; y++) {
        row = fb->pixels + (size_t)y * (size_t)fb->width;
        sy = mask->src.y + (y - dstY);
        for (x = r.x; x < r.x + r.w; x++) {
            sx = mask->src.x + (x - dstX);
            a = mask->alpha[(size_t)sy * (size_t)mask->stride + (size_t)sx];
            row[x] = _ras_blend(row[x], argb, a * colorA / 255);
        }
    }
}

/*
 * Draws an image scaled by an integer zoom (nearest neighbor).
 * Expects: a framebuffer, the clip, the image and its top-left
 *          destination.
 * Returns: nothing.
 */
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
    r.x = dstX;
    r.y = dstY;
    r.w = img->width * zoom;
    r.h = img->height * zoom;
    r = _ras_visible(fb, clip, r);

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

/*
 * Builds the palette lookup table used by ras_blitImage.
 * Expects: an indexed image and lut with room for IMG_PALETTE_MAX values.
 * Returns: nothing (entries beyond the palette are opaque black).
 */
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

#ifdef STANDALONE
/* ================================================================
 * 4. Tests: built with -DSTANDALONE as a test program
 * ================================================================ */

static int _failures = 0;
static int _checks = 0;

#define RAS_CHECK(cond) do { \
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

static void _ras_testImage(RasFb *fb, RasRect all);
static void _ras_testMask(RasFb *fb, RasRect all);

/*
 * Runs the mask and image tests.
 * Expects: nothing.
 * Returns: 0 when every check passes, 1 otherwise.
 */
int main(void)
{
    RasFb fb;
    RasRect all;

    memset(&fb, 0, sizeof(fb));
    ras_resize(&fb, 8, 8);
    all.x = 0;
    all.y = 0;
    all.w = 8;
    all.h = 8;

    _ras_testImage(&fb, all);
    _ras_testMask(&fb, all);

    ras_free(&fb);
    printf("ras_blit: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}

/*
 * Tests a 2x1 indexed image drawn at zoom 2 (covers 4x2 pixels).
 * Expects: an 8x8 framebuffer and a clip covering it.
 * Returns: nothing.
 */
static void _ras_testImage(RasFb *fb, RasRect all)
{
    TkU32 lut[IMG_PALETTE_MAX];
    Img img;
    RasImage view;

    img_create(
        &img,
        2,
        1,
        8
    );
    img.paletteCount = 2;
    img.palette[0] = 0x112233UL;
    img.palette[1] = 0x445566UL;
    img.index[1] = 1;
    ras_buildLut(&img, lut);
    view.img = &img;
    view.lut = lut;
    view.zoom = 2;

    ras_blitImage(
        fb,
        all,
        &view,
        0,
        4
    );
    RAS_CHECK(fb->pixels[4 * 8 + 0] == 0xFF112233UL);
    RAS_CHECK(fb->pixels[5 * 8 + 1] == 0xFF112233UL);
    RAS_CHECK(fb->pixels[4 * 8 + 2] == 0xFF445566UL);
    RAS_CHECK(fb->pixels[5 * 8 + 3] == 0xFF445566UL);
    RAS_CHECK(lut[2] == 0xFF000000UL);
    img_free(&img);
}

/*
 * Tests a 2x1 mask (opaque, transparent) tinted white over black.
 * Expects: an 8x8 framebuffer and a clip covering it.
 * Returns: nothing.
 */
static void _ras_testMask(RasFb *fb, RasRect all)
{
    static const TkU8 alpha[2] = { 255, 0 };
    RasMask mask;

    memset(fb->pixels, 0, 64 * sizeof(TkU32));
    mask.alpha = alpha;
    mask.stride = 2;
    mask.src.x = 0;
    mask.src.y = 0;
    mask.src.w = 2;
    mask.src.h = 1;

    ras_blitMask(
        fb,
        all,
        &mask,
        1,
        0,
        0xFFFFFFFFUL
    );
    RAS_CHECK(fb->pixels[1] == 0xFFFFFFFFUL);
    RAS_CHECK(fb->pixels[2] == 0);              /* alpha 0: untouched */
    RAS_CHECK(fb->pixels[0] == 0);
}

#endif /* STANDALONE */
