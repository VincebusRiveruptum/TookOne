#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

#define BMP_FILE_HDR        14
#define BMP_INFO_MIN        40
#define BMP_MASKS_AT        54      /* bitfield masks, any header >= 40 */
#define BMP_MAX_FILE        (512L * 1024L * 1024L)

#define BMP_BI_RGB          0
#define BMP_BI_BITFIELDS    3
#define BMP_BI_ALPHAFIELDS  6

/* Channel extraction for 16 / 32 bpp. */
typedef struct BmpMask {
    TkU32 mask;
    int shift;
    TkU32 max;
} BmpMask;

/* Parsed header. */
typedef struct BmpInfo {
    int width;
    int height;
    int topDown;
    int bpp;
    long rowSize;
    TkU32 dataOffset;
    TkU32 paletteOffset;
    TkU32 paletteCount;
    BmpMask masks[3];               /* R, G, B */
} BmpInfo;

const char *bmp_strError(int err)
{
    switch (err) {
    case BMP_OK:              return "ok";
    case BMP_ERR_IO:          return "cannot read file";
    case BMP_ERR_NOMEM:       return "out of memory";
    case BMP_ERR_SIGNATURE:   return "not a BMP file";
    case BMP_ERR_FORMAT:      return "malformed BMP file";
    case BMP_ERR_UNSUPPORTED: return "unsupported BMP variant";
    }
    return "unknown error";
}

/* ---- little-endian helpers ---- */

static TkU32 _u16(const TkU8 *p)
{
    return (TkU32)p[0] | ((TkU32)p[1] << 8);
}

static TkU32 _u32(const TkU8 *p)
{
    return (TkU32)p[0]
        | ((TkU32)p[1] << 8)
        | ((TkU32)p[2] << 16)
        | ((TkU32)p[3] << 24);
}

static long _s32(const TkU8 *p)
{
    TkU32 v;

    v = _u32(p);
    if (v & 0x80000000UL)
        return -(long)(~v & 0x7FFFFFFFUL) - 1L;
    return (long)v;
}

/* ---- file ---- */

/* Loads the whole file. Returns BMP_OK and a buffer the caller frees. */
static int _readFile(const char *path, TkU8 **out, long *size)
{
    FILE *fp = NULL;
    TkU8 *buf = NULL;

    *out = NULL;
    fp = fopen(path, "rb");
    if (!fp)
        return BMP_ERR_IO;
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return BMP_ERR_IO;
    }
    *size = ftell(fp);
    if (*size < 0 || *size > BMP_MAX_FILE || fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return BMP_ERR_IO;
    }
    buf = (TkU8 *)malloc(*size > 0 ? (size_t)*size : 1);
    if (!buf) {
        fclose(fp);
        return BMP_ERR_NOMEM;
    }
    if (fread(
        buf,
        1,
        (size_t)*size,
        fp
    ) != (size_t)*size) {
        free(buf);
        fclose(fp);
        return BMP_ERR_IO;
    }
    fclose(fp);
    *out = buf;
    return BMP_OK;
}

/* ---- header ---- */

static void _setMask(BmpMask *m, TkU32 mask)
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

static TkU32 _channel(const BmpMask *m, TkU32 px)
{
    TkU32 v;

    if (m->max == 0)
        return 0;
    v = (px & m->mask) >> m->shift;
    if (v > m->max)
        v = m->max;
    return (v * 255 + m->max / 2) / m->max;
}

static void _setDefaultMasks(BmpInfo *info)
{
    if (info->bpp == 16) {
        _setMask(&info->masks[0], 0x7C00);
        _setMask(&info->masks[1], 0x03E0);
        _setMask(&info->masks[2], 0x001F);
        return;
    }
    _setMask(&info->masks[0], 0x00FF0000UL);
    _setMask(&info->masks[1], 0x0000FF00UL);
    _setMask(&info->masks[2], 0x000000FFUL);
}

static int _parseHeader(const TkU8 *d, long size, BmpInfo *info)
{
    long width;
    long height;
    long need;
    TkU32 hdrSize;
    TkU32 compression;
    TkU32 colors;

    memset(info, 0, sizeof(*info));
    if (size < 2 || d[0] != 'B' || d[1] != 'M')
        return BMP_ERR_SIGNATURE;
    if (size < BMP_FILE_HDR + BMP_INFO_MIN)
        return BMP_ERR_FORMAT;

    info->dataOffset = _u32(d + 10);
    hdrSize = _u32(d + 14);
    if (hdrSize < BMP_INFO_MIN)
        return BMP_ERR_UNSUPPORTED;         /* OS/2 core header */
    if (hdrSize > (TkU32)size - BMP_FILE_HDR)
        return BMP_ERR_FORMAT;

    width = _s32(d + 18);
    height = _s32(d + 22);
    info->bpp = (int)_u16(d + 28);
    compression = _u32(d + 30);
    colors = _u32(d + 46);

    if (_u16(d + 26) != 1)
        return BMP_ERR_FORMAT;
    if (width < 1 || width > IMG_MAX_DIM)
        return BMP_ERR_FORMAT;
    if (height == 0 || height > IMG_MAX_DIM || height < -IMG_MAX_DIM)
        return BMP_ERR_FORMAT;
    info->width = (int)width;
    info->topDown = height < 0;
    info->height = (int)(height < 0 ? -height : height);

    switch (info->bpp) {
    case 1:
    case 4:
    case 8:
        if (compression != BMP_BI_RGB)
            return BMP_ERR_UNSUPPORTED;     /* RLE4 / RLE8 */
        break;
    case 16:
    case 24:
    case 32:
        if (compression == BMP_BI_RGB) {
            _setDefaultMasks(info);
            break;
        }
        if (compression != BMP_BI_BITFIELDS
            && compression != BMP_BI_ALPHAFIELDS)
            return BMP_ERR_UNSUPPORTED;
        if (info->bpp == 24 || size < BMP_MASKS_AT + 12)
            return BMP_ERR_FORMAT;
        _setMask(&info->masks[0], _u32(d + BMP_MASKS_AT));
        _setMask(&info->masks[1], _u32(d + BMP_MASKS_AT + 4));
        _setMask(&info->masks[2], _u32(d + BMP_MASKS_AT + 8));
        break;
    default:
        return BMP_ERR_UNSUPPORTED;
    }

    if (info->bpp <= 8) {
        info->paletteOffset = BMP_FILE_HDR + hdrSize;
        info->paletteCount = colors ? colors : (1UL << info->bpp);
        if (info->paletteCount > (1UL << info->bpp))
            return BMP_ERR_FORMAT;
        need = (long)info->paletteOffset + (long)info->paletteCount * 4;
        if (need > size)
            return BMP_ERR_FORMAT;
    }

    info->rowSize = (((long)info->width * info->bpp + 31) / 32) * 4;
    if (info->dataOffset > (TkU32)size)
        return BMP_ERR_FORMAT;
    if (info->rowSize * info->height > size - (long)info->dataOffset)
        return BMP_ERR_FORMAT;
    return BMP_OK;
}

/* ---- pixels ---- */

static void _readPalette(const TkU8 *d, const BmpInfo *info, Img *img)
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

static void _readIndexedRow(
    const TkU8 *row,
    int bpp,
    int width,
    TkU8 *out
)
{
    int x;
    int shift;
    TkU8 b;

    for (x = 0; x < width; x++) {
        if (bpp == 8) {
            out[x] = row[x];
        } else if (bpp == 4) {
            b = row[x / 2];
            out[x] = (TkU8)((x % 2 == 0) ? b >> 4 : b & 0x0F);
        } else {
            shift = 7 - (x % 8);
            out[x] = (TkU8)((row[x / 8] >> shift) & 1);
        }
    }
}

static void _readDirectRow(
    const TkU8 *row,
    const BmpInfo *info,
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
            px = _u16(row + x * 2);
        else
            px = _u32(row + x * 4);
        out[x] = (_channel(&info->masks[0], px) << 16)
            | (_channel(&info->masks[1], px) << 8)
            | _channel(&info->masks[2], px);
    }
}

int bmp_load(const char *path, Img *img)
{
    int err;
    int y;
    int srcY;
    int imgBpp;
    long size;
    BmpInfo info;
    TkU8 *data = NULL;
    const TkU8 *row = NULL;

    if (!path || !img)
        return BMP_ERR_IO;
    memset(img, 0, sizeof(*img));

    err = _readFile(path, &data, &size);
    if (err != BMP_OK)
        return err;
    err = _parseHeader(data, size, &info);
    if (err != BMP_OK) {
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
        return err == IMG_ERR_NOMEM ? BMP_ERR_NOMEM : BMP_ERR_FORMAT;
    }

    if (img_isIndexed(img))
        _readPalette(data, &info, img);

    for (y = 0; y < info.height; y++) {
        srcY = info.topDown ? y : info.height - 1 - y;
        row = data + info.dataOffset + (long)srcY * info.rowSize;
        if (img_isIndexed(img)) {
            _readIndexedRow(
                row,
                info.bpp,
                info.width,
                img->index + (size_t)y * (size_t)info.width
            );
        } else {
            _readDirectRow(
                row,
                &info,
                img->rgb + (size_t)y * (size_t)info.width
            );
        }
    }

    free(data);
    return BMP_OK;
}
