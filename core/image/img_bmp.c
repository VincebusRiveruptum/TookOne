/*
 * img_bmp - Windows BMP reader (module: image).
 */
#include "private.h"
#include "../../deps/endian/endian.h"

/* ---- Prototypes of file-private functions ---- */
static int _img_readFile(const char *path, TkU8 **out, long *size);
static int _img_parseHeader(const TkU8 *d, long size, ImgBmpInfo *info);
static int _img_parseMasks(const TkU8 *d, long size, ImgBmpInfo *info);
static void _img_readPalette(
    const TkU8 *d,
    const ImgBmpInfo *info,
    Img *img
);
static void _img_readRow(
    const TkU8 *row,
    const ImgBmpInfo *info,
    Img *img,
    int y
);
static void _img_readIndexedRow(
    const TkU8 *row,
    const ImgBmpInfo *info,
    TkU8 *out
);
static void _img_readDirectRow(
    const TkU8 *row,
    const ImgBmpInfo *info,
    TkU32 *out
);
static void _img_setDefaultMasks(ImgBmpInfo *info);
static void _img_setMask(ImgBmpMask *m, TkU32 mask);
static TkU32 _img_channel(const ImgBmpMask *m, TkU32 px);

/* ================================================================
 * 1. Public API
 * ================================================================ */

/*
 * Loads a BMP file into an image.
 * Expects: a path and an Img to fill.
 * Returns: IMG_OK, or an IMG_ERR_* code (img is left empty).
 */
int img_loadBmp(const char *path, Img *img)
{
    int err;
    int y;
    int imgBpp;
    int srcY;
    long size;
    ImgBmpInfo info;
    TkU8 *data = NULL;
    const TkU8 *row = NULL;

    if (!path || !img)
        return IMG_ERR_ARG;
    memset(img, 0, sizeof(*img));

    err = _img_readFile(path, &data, &size);
    if (err != IMG_OK)
        return err;
    err = _img_parseHeader(data, size, &info);
    if (err != IMG_OK) {
        free(data);
        return err;
    }

    imgBpp = info.bpp == 32 ? 24 : info.bpp;
    err = img_create(
        img,
        info.width,
        info.height,
        imgBpp
    );
    if (err != IMG_OK) {
        free(data);
        return err;
    }

    if (img_isIndexed(img))
        _img_readPalette(data, &info, img);
    for (y = 0; y < info.height; y++) {
        srcY = info.topDown ? y : info.height - 1 - y;
        row = data + info.dataOffset + (long)srcY * info.rowSize;
        _img_readRow(
            row,
            &info,
            img,
            y
        );
    }

    free(data);
    return IMG_OK;
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * Loads a whole file into memory.
 * Expects: a path; out and size to receive the buffer and its length.
 * Returns: IMG_OK (caller frees *out), IMG_ERR_IO or IMG_ERR_NOMEM.
 */
static int _img_readFile(const char *path, TkU8 **out, long *size)
{
    FILE *fp = NULL;
    TkU8 *buf = NULL;

    *out = NULL;
    fp = fopen(path, "rb");
    if (!fp)
        return IMG_ERR_IO;
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return IMG_ERR_IO;
    }
    *size = ftell(fp);
    if (*size < 0 || *size > IMG_BMP_MAX_FILE || fseek(fp, 0, SEEK_SET)) {
        fclose(fp);
        return IMG_ERR_IO;
    }
    buf = (TkU8 *)malloc(*size > 0 ? (size_t)*size : 1);
    if (!buf) {
        fclose(fp);
        return IMG_ERR_NOMEM;
    }
    if (fread(
        buf,
        1,
        (size_t)*size,
        fp
    ) != (size_t)*size) {
        free(buf);
        fclose(fp);
        return IMG_ERR_IO;
    }
    fclose(fp);
    *out = buf;
    return IMG_OK;
}

/*
 * Parses and validates the BMP headers.
 * Expects: the whole file in d (size bytes) and an info to fill.
 * Returns: IMG_OK, IMG_ERR_SIGNATURE, IMG_ERR_FORMAT or
 *          IMG_ERR_UNSUPPORTED.
 */
static int _img_parseHeader(const TkU8 *d, long size, ImgBmpInfo *info)
{
    int err;
    long width;
    long height;
    long need;
    TkU32 hdrSize;
    TkU32 colors;

    memset(info, 0, sizeof(*info));
    if (size < 2 || d[0] != 'B' || d[1] != 'M')
        return IMG_ERR_SIGNATURE;
    if (size < IMG_BMP_FILE_HDR + IMG_BMP_INFO_MIN)
        return IMG_ERR_FORMAT;

    info->dataOffset = (TkU32)endianReadU32(d + 10);
    hdrSize = (TkU32)endianReadU32(d + 14);
    if (hdrSize < IMG_BMP_INFO_MIN)
        return IMG_ERR_UNSUPPORTED;         /* OS/2 core header */
    if (hdrSize > (TkU32)size - IMG_BMP_FILE_HDR)
        return IMG_ERR_FORMAT;
    if (endianReadU16(d + 26) != 1)
        return IMG_ERR_FORMAT;              /* planes */

    width = endianReadS32(d + 18);
    height = endianReadS32(d + 22);
    if (width < 1 || width > IMG_MAX_DIM)
        return IMG_ERR_FORMAT;
    if (height == 0 || height > IMG_MAX_DIM || height < -IMG_MAX_DIM)
        return IMG_ERR_FORMAT;
    info->width = (int)width;
    info->topDown = height < 0;
    info->height = (int)(height < 0 ? -height : height);
    info->bpp = (int)endianReadU16(d + 28);

    err = _img_parseMasks(d, size, info);
    if (err != IMG_OK)
        return err;

    if (info->bpp <= 8) {
        colors = (TkU32)endianReadU32(d + 46);
        info->paletteOffset = IMG_BMP_FILE_HDR + hdrSize;
        info->paletteCount = colors ? colors : (1UL << info->bpp);
        if (info->paletteCount > (1UL << info->bpp))
            return IMG_ERR_FORMAT;
        need = (long)info->paletteOffset + (long)info->paletteCount * 4;
        if (need > size)
            return IMG_ERR_FORMAT;
    }

    info->rowSize = (((long)info->width * info->bpp + 31) / 32) * 4;
    if (info->dataOffset > (TkU32)size)
        return IMG_ERR_FORMAT;
    if (info->rowSize * info->height > size - (long)info->dataOffset)
        return IMG_ERR_FORMAT;
    return IMG_OK;
}

/*
 * Checks the compression for the depth and sets the channel masks.
 * Expects: the file in d (size bytes) and info with bpp already set.
 * Returns: IMG_OK, IMG_ERR_FORMAT or IMG_ERR_UNSUPPORTED.
 */
static int _img_parseMasks(const TkU8 *d, long size, ImgBmpInfo *info)
{
    TkU32 compression;

    compression = (TkU32)endianReadU32(d + 30);
    switch (info->bpp) {
    case 1:
    case 4:
    case 8:
        if (compression != IMG_BMP_BI_RGB)
            return IMG_ERR_UNSUPPORTED;     /* RLE4 / RLE8 */
        return IMG_OK;
    case 16:
    case 24:
    case 32:
        break;
    default:
        return IMG_ERR_UNSUPPORTED;
    }

    if (compression == IMG_BMP_BI_RGB) {
        _img_setDefaultMasks(info);
        return IMG_OK;
    }
    if (compression != IMG_BMP_BI_BITFIELDS
        && compression != IMG_BMP_BI_ALPHAFIELDS)
        return IMG_ERR_UNSUPPORTED;
    if (info->bpp == 24 || size < IMG_BMP_MASKS_AT + 12)
        return IMG_ERR_FORMAT;
    _img_setMask(&info->masks[0], (TkU32)endianReadU32(d + 54));
    _img_setMask(&info->masks[1], (TkU32)endianReadU32(d + 58));
    _img_setMask(&info->masks[2], (TkU32)endianReadU32(d + 62));
    return IMG_OK;
}

/*
 * Copies the BMP palette (B, G, R, x entries) into the image.
 * Expects: a parsed info of an indexed BMP and the indexed image.
 * Returns: nothing.
 */
static void _img_readPalette(
    const TkU8 *d,
    const ImgBmpInfo *info,
    Img *img
)
{
    TkU32 i;
    const TkU8 *p = NULL;

    img->paletteCount = (int)info->paletteCount;
    for (i = 0; i < info->paletteCount; i++) {
        p = d + info->paletteOffset + i * 4;
        img->palette[i] = ((TkU32)p[2] << 16)
            | ((TkU32)p[1] << 8)
            | (TkU32)p[0];
    }
}

/*
 * Decodes one file row into image row y.
 * Expects: a row of info->rowSize bytes and a created image.
 * Returns: nothing.
 */
static void _img_readRow(
    const TkU8 *row,
    const ImgBmpInfo *info,
    Img *img,
    int y
)
{
    size_t start;

    start = (size_t)y * (size_t)info->width;
    if (img_isIndexed(img))
        _img_readIndexedRow(row, info, img->index + start);
    else
        _img_readDirectRow(row, info, img->rgb + start);
}

/*
 * Unpacks a 1/4/8 bpp row into one index per byte.
 * Expects: a file row and out with room for info->width bytes.
 * Returns: nothing.
 */
static void _img_readIndexedRow(
    const TkU8 *row,
    const ImgBmpInfo *info,
    TkU8 *out
)
{
    int x;
    int shift;
    TkU8 b;

    for (x = 0; x < info->width; x++) {
        if (info->bpp == 8) {
            out[x] = row[x];
        } else if (info->bpp == 4) {
            b = row[x / 2];
            out[x] = (TkU8)((x % 2 == 0) ? b >> 4 : b & 0x0F);
        } else {
            shift = 7 - (x % 8);
            out[x] = (TkU8)((row[x / 8] >> shift) & 1);
        }
    }
}

/*
 * Converts a 16/24/32 bpp row to 0x00RRGGBB values.
 * Expects: a file row and out with room for info->width values.
 * Returns: nothing.
 */
static void _img_readDirectRow(
    const TkU8 *row,
    const ImgBmpInfo *info,
    TkU32 *out
)
{
    int x;
    TkU32 px;
    const TkU8 *p = NULL;

    for (x = 0; x < info->width; x++) {
        if (info->bpp == 24) {
            p = row + x * 3;
            out[x] = ((TkU32)p[2] << 16) | ((TkU32)p[1] << 8) | p[0];
            continue;
        }
        if (info->bpp == 16)
            px = (TkU32)endianReadU16(row + x * 2);
        else
            px = (TkU32)endianReadU32(row + x * 4);
        out[x] = (_img_channel(&info->masks[0], px) << 16)
            | (_img_channel(&info->masks[1], px) << 8)
            | _img_channel(&info->masks[2], px);
    }
}

/*
 * Sets the masks used by uncompressed 16 (555) and 32 bpp files.
 * Expects: info with bpp 16, 24 or 32.
 * Returns: nothing.
 */
static void _img_setDefaultMasks(ImgBmpInfo *info)
{
    if (info->bpp == 16) {
        _img_setMask(&info->masks[0], 0x7C00);
        _img_setMask(&info->masks[1], 0x03E0);
        _img_setMask(&info->masks[2], 0x001F);
        return;
    }
    _img_setMask(&info->masks[0], 0x00FF0000UL);
    _img_setMask(&info->masks[1], 0x0000FF00UL);
    _img_setMask(&info->masks[2], 0x000000FFUL);
}

/*
 * Prepares a channel mask for extraction.
 * Expects: a contiguous bit mask (0 = channel absent).
 * Returns: nothing.
 */
static void _img_setMask(ImgBmpMask *m, TkU32 mask)
{
    TkU32 v;

    m->mask = mask;
    m->shift = 0;
    m->max = 0;
    if (mask == 0)
        return;
    v = mask;
    while ((v & 1) == 0) {
        v >>= 1;
        m->shift++;
    }
    m->max = v;
}

/*
 * Extracts one channel of a pixel, scaled to 0..255.
 * Expects: a prepared mask and a raw pixel value.
 * Returns: the 8-bit channel value (0 if the channel is absent).
 */
static TkU32 _img_channel(const ImgBmpMask *m, TkU32 px)
{
    TkU32 v;

    if (m->max == 0)
        return 0;
    v = (px & m->mask) >> m->shift;
    if (v > m->max)
        v = m->max;
    return (v * 255 + m->max / 2) / m->max;
}

#ifdef STANDALONE
/* ================================================================
 * 4. Tests: built with -DSTANDALONE as a test program
 * ================================================================ */

#define IMG_TEST_FILE   "imgtest.bmp"

/* Header fields of a test BMP. */
typedef struct ImgTestBmp {
    int width;
    int height;                 /* negative = top-down */
    int bpp;
    int colors;                 /* palette entries (bpp <= 8) */
    unsigned long compression;
    unsigned long masks[3];     /* written when compression == 3 */
} ImgTestBmp;

static int _failures = 0;
static int _checks = 0;

#define IMG_CHECK(cond) do { \
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

static void _img_testIndexed(void);
static void _img_testLowDepths(void);
static void _img_testDirect(void);
static void _img_testErrors(void);
static ImgTestBmp _img_testSpec(const int *whbc);
static void _img_testWrite(
    const ImgTestBmp *spec,
    const TkU8 *palette,
    const TkU8 *pixels,
    long pixelBytes
);
static void _img_testPut(unsigned char *p, unsigned long v, int bytes);

/*
 * Runs the BMP reader tests.
 * Expects: a writable current directory.
 * Returns: 0 when every check passes, 1 otherwise.
 */
int main(void)
{
    _img_testIndexed();
    _img_testLowDepths();
    _img_testDirect();
    _img_testErrors();
    remove(IMG_TEST_FILE);

    printf("img_bmp: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}

/*
 * Tests 8 bpp files: palette, row order, bottom-up and top-down.
 * Expects: nothing.
 * Returns: nothing.
 */
static void _img_testIndexed(void)
{
    static const int dims[4] = { 3, 2, 8, 3 };
    /* B, G, R, 0 */
    static const TkU8 pal[12] = {
        0, 0, 255, 0,   0, 255, 0, 0,   255, 0, 0, 0
    };
    /* 3x2, rows padded to 4, bottom row first */
    static const TkU8 px[8] = { 2, 1, 0, 0,   0, 1, 2, 0 };
    ImgTestBmp s;
    Img img;

    s = _img_testSpec(dims);
    _img_testWrite(
        &s,
        pal,
        px,
        8
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_OK);
    IMG_CHECK(img.width == 3 && img.height == 2 && img.bpp == 8);
    IMG_CHECK(img.paletteCount == 3);
    IMG_CHECK(img.palette[0] == 0xFF0000UL);     /* red */
    IMG_CHECK(img.palette[2] == 0x0000FFUL);     /* blue */
    /* the top row is the last one in the file */
    IMG_CHECK(img.index[0] == 0 && img.index[1] == 1 && img.index[2] == 2);
    IMG_CHECK(img.index[3] == 2 && img.index[4] == 1 && img.index[5] == 0);
    img_free(&img);

    s.height = -2;                              /* top-down */
    _img_testWrite(
        &s,
        pal,
        px,
        8
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_OK);
    IMG_CHECK(img.index[0] == 2 && img.index[5] == 2);
    img_free(&img);
}

/*
 * Tests 1 and 4 bpp bit unpacking.
 * Expects: nothing.
 * Returns: nothing.
 */
static void _img_testLowDepths(void)
{
    static const int dims1[4] = { 3, 1, 1, 2 };
    static const int dims4[4] = { 3, 1, 4, 16 };
    static const TkU8 pal2[8] = { 0, 0, 0, 0,   255, 255, 255, 0 };
    static const TkU8 px1[4] = { 0xA0, 0, 0, 0 };     /* 1 0 1 */
    static const TkU8 px4[4] = { 0x1F, 0x20, 0, 0 };  /* 1 F 2 */
    TkU8 pal16[64];
    ImgTestBmp s;
    Img img;

    s = _img_testSpec(dims1);
    _img_testWrite(
        &s,
        pal2,
        px1,
        4
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_OK);
    IMG_CHECK(img.bpp == 1);
    IMG_CHECK(img.index[0] == 1 && img.index[1] == 0 && img.index[2] == 1);
    img_free(&img);

    memset(pal16, 0, sizeof(pal16));
    s = _img_testSpec(dims4);
    _img_testWrite(
        &s,
        pal16,
        px4,
        4
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_OK);
    IMG_CHECK(img.index[0] == 1 && img.index[1] == 15 && img.index[2] == 2);
    img_free(&img);
}

/*
 * Tests 24 bpp and 16 bpp (565 bitfields) color conversion.
 * Expects: nothing.
 * Returns: nothing.
 */
static void _img_testDirect(void)
{
    static const int dims24[4] = { 2, 1, 24, 0 };
    static const int dims16[4] = { 2, 1, 16, 0 };
    /* 24 bpp, 2x1: B, G, R pixels, padded to 8 */
    static const TkU8 px24[8] = { 1, 2, 3,   4, 5, 6,   0, 0 };
    /* 16 bpp 565: pure red (0xF800), pure blue (0x001F) */
    static const TkU8 px16[4] = { 0x00, 0xF8, 0x1F, 0x00 };
    ImgTestBmp s;
    Img img;

    s = _img_testSpec(dims24);
    _img_testWrite(
        &s,
        NULL,
        px24,
        8
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_OK);
    IMG_CHECK(img.bpp == 24);
    IMG_CHECK(img.rgb[0] == 0x030201UL && img.rgb[1] == 0x060504UL);
    img_free(&img);

    s = _img_testSpec(dims16);
    s.compression = 3;
    s.masks[0] = 0xF800;
    s.masks[1] = 0x07E0;
    s.masks[2] = 0x001F;
    _img_testWrite(
        &s,
        NULL,
        px16,
        4
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_OK);
    IMG_CHECK(img.bpp == 16);
    IMG_CHECK(img.rgb[0] == 0xFF0000UL && img.rgb[1] == 0x0000FFUL);
    img_free(&img);
}

/*
 * Tests the error codes for missing, foreign and malformed files.
 * Expects: nothing.
 * Returns: nothing.
 */
static void _img_testErrors(void)
{
    static const int rle[4] = { 4, 1, 8, 256 };
    static const int big[4] = { 64, 64, 8, 256 };
    static const int zero[4] = { 0, 1, 8, 256 };
    static const TkU8 px[4] = { 0, 0, 0, 0 };
    TkU8 pal[1024];
    ImgTestBmp s;
    Img img;
    FILE *fp = NULL;

    memset(pal, 0, sizeof(pal));
    IMG_CHECK(img_loadBmp("does-not-exist.bmp", &img) == IMG_ERR_IO);

    fp = fopen(IMG_TEST_FILE, "wb");
    fputs("PK not a bitmap", fp);
    fclose(fp);
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_ERR_SIGNATURE);

    s = _img_testSpec(rle);
    s.compression = 1;                          /* RLE8 */
    _img_testWrite(
        &s,
        pal,
        px,
        4
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_ERR_UNSUPPORTED);

    s = _img_testSpec(big);                     /* pixel data missing */
    _img_testWrite(
        &s,
        pal,
        px,
        4
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_ERR_FORMAT);
    IMG_CHECK(img.index == NULL && img.rgb == NULL);

    s = _img_testSpec(zero);                    /* zero width */
    _img_testWrite(
        &s,
        pal,
        px,
        4
    );
    IMG_CHECK(img_loadBmp(IMG_TEST_FILE, &img) == IMG_ERR_FORMAT);
}

/*
 * Builds a test header description.
 * Expects: { width, height, bpp, palette colors }.
 * Returns: the description, uncompressed and without masks.
 */
static ImgTestBmp _img_testSpec(const int *whbc)
{
    ImgTestBmp s;

    memset(&s, 0, sizeof(s));
    s.width = whbc[0];
    s.height = whbc[1];
    s.bpp = whbc[2];
    s.colors = whbc[3];
    return s;
}

/*
 * Writes IMG_TEST_FILE with the given header, palette and pixel rows.
 * Expects: palette of spec->colors entries; rows already padded.
 * Returns: nothing (exits if the file cannot be written).
 */
static void _img_testWrite(
    const ImgTestBmp *spec,
    const TkU8 *palette,
    const TkU8 *pixels,
    long pixelBytes
)
{
    int i;
    long hdr;
    long size;
    TkU8 head[66];
    FILE *fp = NULL;

    hdr = 54 + (spec->compression == 3 ? 12 : 0);
    size = hdr + spec->colors * 4 + pixelBytes;
    memset(head, 0, sizeof(head));
    head[0] = 'B';
    head[1] = 'M';
    _img_testPut(head + 2, (unsigned long)size, 4);
    _img_testPut(head + 10, (unsigned long)(hdr + spec->colors * 4), 4);
    _img_testPut(head + 14, 40, 4);
    _img_testPut(head + 18, (unsigned long)spec->width, 4);
    _img_testPut(head + 22, (unsigned long)spec->height, 4);
    _img_testPut(head + 26, 1, 2);
    _img_testPut(head + 28, (unsigned long)spec->bpp, 2);
    _img_testPut(head + 30, spec->compression, 4);
    _img_testPut(head + 46, (unsigned long)spec->colors, 4);
    for (i = 0; i < 3; i++)
        _img_testPut(head + 54 + i * 4, spec->masks[i], 4);

    fp = fopen(IMG_TEST_FILE, "wb");
    if (!fp) {
        printf("cannot write %s\n", IMG_TEST_FILE);
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
}

/*
 * Stores v as a little-endian value of 2 or 4 bytes.
 * Expects: p with room for bytes bytes.
 * Returns: nothing.
 */
static void _img_testPut(unsigned char *p, unsigned long v, int bytes)
{
    int i;

    for (i = 0; i < bytes; i++)
        p[i] = (unsigned char)((v >> (8 * i)) & 0xFF);
}

#endif /* STANDALONE */
