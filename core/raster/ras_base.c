/*
 * ras_base - framebuffer, rectangles and fills (module: raster).
 */
#include "private.h"

/* ================================================================
 * 1. Public API
 * ================================================================ */

/*
 * Allocates (or reallocates) the framebuffer pixels.
 * Expects: a framebuffer (zeroed the first time) and a size >= 1.
 * Returns: RAS_OK, RAS_ERR_ARG or RAS_ERR_NOMEM (fb is unchanged).
 */
int ras_resize(RasFb *fb, int width, int height)
{
    size_t bytes;
    TkU32 *pixels = NULL;

    if (!fb || width < 1 || height < 1)
        return RAS_ERR_ARG;
    if (fb->pixels && fb->width == width && fb->height == height)
        return RAS_OK;

    bytes = (size_t)width * (size_t)height * sizeof(TkU32);
    pixels = (TkU32 *)malloc(bytes);
    if (!pixels)
        return RAS_ERR_NOMEM;
    free(fb->pixels);
    fb->pixels = pixels;
    fb->width = width;
    fb->height = height;
    return RAS_OK;
}

/*
 * Releases the framebuffer pixels and leaves it empty.
 * Expects: a framebuffer (possibly empty).
 * Returns: nothing.
 */
void ras_free(RasFb *fb)
{
    if (!fb)
        return;
    free(fb->pixels);
    memset(fb, 0, sizeof(*fb));
}

/*
 * Builds a rectangle.
 * Expects: position and size (a size <= 0 means empty).
 * Returns: the rectangle.
 */
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

/*
 * Intersects two rectangles.
 * Expects: any two rectangles.
 * Returns: the common area (w or h is 0 when they do not overlap).
 */
RasRect ras_intersect(RasRect a, RasRect b)
{
    int x0;
    int y0;
    int x1;
    int y1;
    RasRect r;

    x0 = a.x > b.x ? a.x : b.x;
    y0 = a.y > b.y ? a.y : b.y;
    x1 = a.x + a.w < b.x + b.w ? a.x + a.w : b.x + b.w;
    y1 = a.y + a.h < b.y + b.h ? a.y + a.h : b.y + b.h;
    r.x = x0;
    r.y = y0;
    r.w = x1 > x0 ? x1 - x0 : 0;
    r.h = y1 > y0 ? y1 - y0 : 0;
    return r;
}

/*
 * Fills a rectangle with a color; alpha below 255 blends.
 * Expects: a framebuffer, the clip, the rectangle and a 0xAARRGGBB color.
 * Returns: nothing.
 */
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

    r = _ras_visible(fb, clip, rect);
    a = (argb >> 24) & 0xFF;
    for (y = r.y; y < r.y + r.h; y++) {
        row = fb->pixels + (size_t)y * (size_t)fb->width;
        for (x = r.x; x < r.x + r.w; x++)
            row[x] = _ras_blend(row[x], argb, a);
    }
}

/* ================================================================
 * 2. Module internal (declared in private.h)
 * ================================================================ */

/*
 * Computes the part of a rectangle that can be drawn.
 * Expects: a framebuffer, the clip and the rectangle.
 * Returns: rect intersected with clip and with the framebuffer.
 */
RasRect _ras_visible(const RasFb *fb, RasRect clip, RasRect rect)
{
    RasRect r;
    RasRect all;

    all.x = 0;
    all.y = 0;
    all.w = fb->width;
    all.h = fb->height;
    r = ras_intersect(rect, clip);
    return ras_intersect(r, all);
}

/*
 * Blends a color over another.
 * Expects: two 0xAARRGGBB colors and the alpha of src (0..255).
 * Returns: the opaque result.
 */
TkU32 _ras_blend(TkU32 dst, TkU32 src, TkU32 a)
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

static RasRect _ras_testRect(const int *xywh);

/*
 * Runs the framebuffer, rectangle and fill tests.
 * Expects: nothing.
 * Returns: 0 when every check passes, 1 otherwise.
 */
int main(void)
{
    static const int full[4] = { 0, 0, 8, 8 };
    static const int corner[4] = { 0, 0, 2, 2 };
    static const int outside[4] = { 6, 6, 10, 10 };
    static const int dot[4] = { 3, 3, 1, 1 };
    static const int boxA[4] = { 0, 0, 4, 4 };
    static const int boxB[4] = { 2, 2, 4, 4 };
    static const int boxC[4] = { 5, 5, 2, 2 };
    RasFb fb;
    RasRect all;
    RasRect r;

    memset(&fb, 0, sizeof(fb));
    RAS_CHECK(ras_resize(&fb, 8, 8) == RAS_OK);
    RAS_CHECK(ras_resize(&fb, 0, 8) == RAS_ERR_ARG);
    all = _ras_testRect(full);
    ras_fillRect(
        &fb,
        all,
        all,
        0xFF000000UL
    );

    /* the clip limits the fill */
    ras_fillRect(
        &fb,
        _ras_testRect(corner),
        all,
        0xFFFFFFFFUL
    );
    RAS_CHECK(fb.pixels[0] == 0xFFFFFFFFUL);
    RAS_CHECK(fb.pixels[2] == 0xFF000000UL);

    /* a rectangle partly outside the framebuffer */
    ras_fillRect(
        &fb,
        all,
        _ras_testRect(outside),
        0xFF00FF00UL
    );
    RAS_CHECK(fb.pixels[7 * 8 + 7] == 0xFF00FF00UL);

    /* 50% white over black */
    ras_fillRect(
        &fb,
        all,
        _ras_testRect(dot),
        0x80FFFFFFUL
    );
    RAS_CHECK((fb.pixels[3 * 8 + 3] & 0xFF) == 0x80);

    r = ras_intersect(_ras_testRect(boxA), _ras_testRect(boxB));
    RAS_CHECK(r.x == 2 && r.y == 2 && r.w == 2 && r.h == 2);
    r = ras_intersect(_ras_testRect(corner), _ras_testRect(boxC));
    RAS_CHECK(r.w == 0 && r.h == 0);

    ras_free(&fb);
    RAS_CHECK(fb.pixels == NULL);

    printf("ras_base: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}

/*
 * Builds a rectangle from an array.
 * Expects: { x, y, w, h }.
 * Returns: the rectangle.
 */
static RasRect _ras_testRect(const int *xywh)
{
    RasRect r;

    r.x = xywh[0];
    r.y = xywh[1];
    r.w = xywh[2];
    r.h = xywh[3];
    return r;
}

#endif /* STANDALONE */
