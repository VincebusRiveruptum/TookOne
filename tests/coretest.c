/*
 * coretest - console tests for core/ and ui/font (no window needed).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../core/bmp.h"
#include "../core/raster.h"
#include "../ui/font.h"

#define CT_FILE     "cttmp.bmp"

static int _failures = 0;
static int _checks = 0;

#define CT_CHECK(cond) do { \
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

/* ---- BMP builder ---- */

static void _put16(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)(v & 0xFF);
    p[1] = (unsigned char)((v >> 8) & 0xFF);
}

static void _put32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)(v & 0xFF);
    p[1] = (unsigned char)((v >> 8) & 0xFF);
    p[2] = (unsigned char)((v >> 16) & 0xFF);
    p[3] = (unsigned char)((v >> 24) & 0xFF);
}

typedef struct CtBmp {
    int width;
    int height;             /* negative = top-down */
    int bpp;
    int colors;             /* palette entries (bpp <= 8) */
    unsigned long compression;
    unsigned long masks[3]; /* written when compression == 3 */
} CtBmp;

/*
 * Writes a BMP with the given header and pixel bytes (rows already padded,
 * in file order). Returns the file size.
 */
static long _writeBmp(
    const CtBmp *spec,
    const unsigned char *palette,
    const unsigned char *pixels,
    long pixelBytes
)
{
    int i;
    long hdr;
    long size;
    unsigned char head[66];
    FILE *fp = NULL;

    hdr = 54 + (spec->compression == 3 ? 12 : 0);
    size = hdr + spec->colors * 4 + pixelBytes;
    memset(head, 0, sizeof(head));
    head[0] = 'B';
    head[1] = 'M';
    _put32(head + 2, (unsigned long)size);
    _put32(head + 10, (unsigned long)(hdr + spec->colors * 4));
    _put32(head + 14, 40);
    _put32(head + 18, (unsigned long)spec->width);
    _put32(head + 22, (unsigned long)spec->height);
    _put16(head + 26, 1);
    _put16(head + 28, (unsigned long)spec->bpp);
    _put32(head + 30, spec->compression);
    _put32(head + 46, (unsigned long)spec->colors);
    for (i = 0; i < 3; i++)
        _put32(head + 54 + i * 4, spec->masks[i]);

    fp = fopen(CT_FILE, "wb");
    if (!fp) {
        printf("cannot write %s\n", CT_FILE);
        exit(2);
    }
    fwrite(
        head,
        1,
        (size_t)hdr,
        fp
    );
    if (spec->colors > 0)
        fwrite(
            palette,
            4,
            (size_t)spec->colors,
            fp
        );
    fwrite(
        pixels,
        1,
        (size_t)pixelBytes,
        fp
    );
    fclose(fp);
    return size;
}

static CtBmp _spec(
    int width,
    int height,
    int bpp,
    int colors
)
{
    CtBmp s;

    memset(&s, 0, sizeof(s));
    s.width = width;
    s.height = height;
    s.bpp = bpp;
    s.colors = colors;
    return s;
}

/* ---- BMP tests ---- */

static void _testBmp8(void)
{
    /* B, G, R, 0 */
    static const unsigned char pal[12] = {
        0, 0, 255, 0,   0, 255, 0, 0,   255, 0, 0, 0
    };
    /* 3x2, bottom-up rows padded to 4: bottom row first */
    static const unsigned char px[8] = { 2, 1, 0, 0,   0, 1, 2, 0 };
    CtBmp s;
    Img img;

    s = _spec(
        3,
        2,
        8,
        3
    );
    _writeBmp(
        &s,
        pal,
        px,
        8
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_OK);
    CT_CHECK(img.width == 3 && img.height == 2 && img.bpp == 8);
    CT_CHECK(img.paletteCount == 3);
    CT_CHECK(img.palette[0] == 0xFF0000UL);     /* red */
    CT_CHECK(img.palette[2] == 0x0000FFUL);     /* blue */
    /* top row is the last one in the file */
    CT_CHECK(img.index[0] == 0 && img.index[1] == 1 && img.index[2] == 2);
    CT_CHECK(img.index[3] == 2 && img.index[4] == 1 && img.index[5] == 0);
    img_free(&img);

    s.height = -2;                              /* top-down */
    _writeBmp(
        &s,
        pal,
        px,
        8
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_OK);
    CT_CHECK(img.index[0] == 2 && img.index[5] == 2);
    img_free(&img);
}

static void _testBmp1And4(void)
{
    static const unsigned char pal2[8] = { 0, 0, 0, 0,   255, 255, 255, 0 };
    static const unsigned char px1[4] = { 0xA0, 0, 0, 0 };     /* 1 0 1 */
    static const unsigned char px4[4] = { 0x1F, 0x20, 0, 0 };  /* 1 F 2 */
    unsigned char pal16[64];
    CtBmp s;
    Img img;

    s = _spec(
        3,
        1,
        1,
        2
    );
    _writeBmp(
        &s,
        pal2,
        px1,
        4
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_OK);
    CT_CHECK(img.bpp == 1);
    CT_CHECK(img.index[0] == 1 && img.index[1] == 0 && img.index[2] == 1);
    img_free(&img);

    memset(pal16, 0, sizeof(pal16));
    s = _spec(
        3,
        1,
        4,
        16
    );
    _writeBmp(
        &s,
        pal16,
        px4,
        4
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_OK);
    CT_CHECK(img.index[0] == 1 && img.index[1] == 15 && img.index[2] == 2);
    img_free(&img);
}

static void _testBmpDirect(void)
{
    /* 24 bpp, 2x1: B,G,R pixels, padded to 8 */
    static const unsigned char px24[8] = { 1, 2, 3,   4, 5, 6,   0, 0 };
    /* 16 bpp 565: pure red (0xF800), pure blue (0x001F) */
    static const unsigned char px16[4] = { 0x00, 0xF8, 0x1F, 0x00 };
    CtBmp s;
    Img img;

    s = _spec(
        2,
        1,
        24,
        0
    );
    _writeBmp(
        &s,
        NULL,
        px24,
        8
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_OK);
    CT_CHECK(img.bpp == 24);
    CT_CHECK(img.rgb[0] == 0x030201UL && img.rgb[1] == 0x060504UL);
    img_free(&img);

    s = _spec(
        2,
        1,
        16,
        0
    );
    s.compression = 3;
    s.masks[0] = 0xF800;
    s.masks[1] = 0x07E0;
    s.masks[2] = 0x001F;
    _writeBmp(
        &s,
        NULL,
        px16,
        4
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_OK);
    CT_CHECK(img.bpp == 16);
    CT_CHECK(img.rgb[0] == 0xFF0000UL && img.rgb[1] == 0x0000FFUL);
    img_free(&img);
}

static void _testBmpErrors(void)
{
    static const unsigned char px[4] = { 0, 0, 0, 0 };
    unsigned char pal[1024];
    CtBmp s;
    Img img;
    FILE *fp = NULL;

    memset(pal, 0, sizeof(pal));
    CT_CHECK(bmp_load("does-not-exist.bmp", &img) == BMP_ERR_IO);

    fp = fopen(CT_FILE, "wb");
    fputs("PK not a bitmap", fp);
    fclose(fp);
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_ERR_SIGNATURE);

    s = _spec(
        4,
        1,
        8,
        256
    );
    s.compression = 1;                          /* RLE8 */
    _writeBmp(
        &s,
        pal,
        px,
        4
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_ERR_UNSUPPORTED);

    /* pixel data missing */
    s = _spec(
        64,
        64,
        8,
        256
    );
    _writeBmp(
        &s,
        pal,
        px,
        4
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_ERR_FORMAT);
    CT_CHECK(img.index == NULL && img.rgb == NULL);

    /* zero width */
    s = _spec(
        0,
        1,
        8,
        256
    );
    _writeBmp(
        &s,
        pal,
        px,
        4
    );
    CT_CHECK(bmp_load(CT_FILE, &img) == BMP_ERR_FORMAT);
}

/* ---- raster tests ---- */

/* x, y, w, h -> RasRect */
static RasRect _rectOf(const int *v)
{
    RasRect r;

    r.x = v[0];
    r.y = v[1];
    r.w = v[2];
    r.h = v[3];
    return r;
}

static void _testRaster(void)
{
    static const int boxA[4] = { 0, 0, 4, 4 };
    static const int boxB[4] = { 2, 2, 4, 4 };
    static const int boxC[4] = { 0, 0, 2, 2 };
    static const int boxD[4] = { 5, 5, 2, 2 };
    TkU32 lut[IMG_PALETTE_MAX];
    RasFb fb;
    RasRect all;
    RasRect r;
    Img img;
    RasImage view;

    memset(&fb, 0, sizeof(fb));
    CT_CHECK(ras_resize(&fb, 8, 8) == RAS_OK);
    all = ras_rect(
        0,
        0,
        8,
        8
    );
    ras_fillRect(
        &fb,
        all,
        all,
        0xFF000000UL
    );

    /* clip limits the fill */
    ras_fillRect(
        &fb,
        ras_rect(
            0,
            0,
            2,
            2
        ),
        all,
        0xFFFFFFFFUL
    );
    CT_CHECK(fb.pixels[0] == 0xFFFFFFFFUL);
    CT_CHECK(fb.pixels[2] == 0xFF000000UL);

    /* rect partly outside the framebuffer */
    ras_fillRect(
        &fb,
        all,
        ras_rect(
            6,
            6,
            10,
            10
        ),
        0xFF00FF00UL
    );
    CT_CHECK(fb.pixels[7 * 8 + 7] == 0xFF00FF00UL);

    /* 50% blend of white over black */
    ras_fillRect(
        &fb,
        all,
        ras_rect(
            3,
            3,
            1,
            1
        ),
        0x80FFFFFFUL
    );
    CT_CHECK((fb.pixels[3 * 8 + 3] & 0xFF) == 0x80);

    r = ras_intersect(_rectOf(boxA), _rectOf(boxB));
    CT_CHECK(r.x == 2 && r.y == 2 && r.w == 2 && r.h == 2);
    r = ras_intersect(_rectOf(boxC), _rectOf(boxD));
    CT_CHECK(r.w == 0 && r.h == 0);

    /* 2x1 indexed image at zoom 2 covers 4x2 pixels */
    CT_CHECK(img_create(
        &img,
        2,
        1,
        8
    ) == IMG_OK);
    img.paletteCount = 2;
    img.palette[0] = 0x112233UL;
    img.palette[1] = 0x445566UL;
    img.index[1] = 1;
    ras_buildLut(&img, lut);
    view.img = &img;
    view.lut = lut;
    view.zoom = 2;
    ras_blitImage(
        &fb,
        all,
        &view,
        0,
        4
    );
    CT_CHECK(fb.pixels[4 * 8 + 0] == 0xFF112233UL);
    CT_CHECK(fb.pixels[5 * 8 + 1] == 0xFF112233UL);
    CT_CHECK(fb.pixels[4 * 8 + 2] == 0xFF445566UL);
    CT_CHECK(fb.pixels[5 * 8 + 3] == 0xFF445566UL);
    CT_CHECK(lut[2] == 0xFF000000UL);

    img_free(&img);
    ras_free(&fb);
}

static void _testFont(void)
{
    RasMask a;
    RasMask b;

    font_glyph('a', &a);
    font_glyph('b', &b);
    CT_CHECK(a.src.w > 0 && a.src.h > 0);
    CT_CHECK(font_textWidth("ab", -1) == a.src.w + b.src.w);
    CT_CHECK(font_textWidth("ab", 1) == a.src.w);
    /* a UTF-8 character takes one glyph */
    CT_CHECK(font_textWidth("\xC3\xA1", -1) == font_textWidth("\x7F", -1));
    CT_CHECK(font_icon(FONT_ICON_CHECK, &a) == 1);
    CT_CHECK(font_icon(99, &a) == 0);
}

int main(void)
{
    _testBmp8();
    _testBmp1And4();
    _testBmpDirect();
    _testBmpErrors();
    _testRaster();
    _testFont();
    remove(CT_FILE);

    printf("core: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}
