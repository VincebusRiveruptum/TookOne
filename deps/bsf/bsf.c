/*
 * bsf - Bitmap Sequence File reader / writer. Spec: docs/bsf_format.md
 *
 * Build the tests with -DSTANDALONE (see makefile / makefile.wat).
 */
#include <stdlib.h>
#include <string.h>
#include "bsf.h"

#define BSF_HEAD_SIZE       18
#define BSF_HEAD_MAX        256     /* newer revisions may append fields */
#define BSF_NAME_MAX_READ   4096
#define BSF_FRAM_HDR        4
#define BSF_RLE_MAX_LITERAL 128
#define BSF_RLE_MAX_RUN     129

static const char _signature[8] = { 'T', 'O', 'O', 'K', 'B', 'S', 'F', ' ' };

static BsfU32 _crcTable[256];
static int _crcReady = 0;

/* ======================================================================
 * Little-endian helpers
 * ==================================================================== */

static void _putU16(BsfU8 *p, BsfU16 v)
{
    p[0] = (BsfU8)(v & 0xFF);
    p[1] = (BsfU8)(v >> 8);
}

static void _putU32(BsfU8 *p, BsfU32 v)
{
    p[0] = (BsfU8)(v & 0xFF);
    p[1] = (BsfU8)((v >> 8) & 0xFF);
    p[2] = (BsfU8)((v >> 16) & 0xFF);
    p[3] = (BsfU8)((v >> 24) & 0xFF);
}

static BsfU16 _getU16(const BsfU8 *p)
{
    return (BsfU16)(p[0] | (p[1] << 8));
}

static BsfU32 _getU32(const BsfU8 *p)
{
    return (BsfU32)p[0]
        | ((BsfU32)p[1] << 8)
        | ((BsfU32)p[2] << 16)
        | ((BsfU32)p[3] << 24);
}

/* Returns 1 when all len bytes were transferred. */
static int _writeBytes(FILE *fp, const void *data, BsfU32 len)
{
    if (len == 0)
        return 1;
    return fwrite(
        data,
        1,
        len,
        fp
    ) == len;
}

static int _readBytes(FILE *fp, void *data, BsfU32 len)
{
    if (len == 0)
        return 1;
    return fread(
        data,
        1,
        len,
        fp
    ) == len;
}

/* ======================================================================
 * CRC-32 (IEEE 802.3, same as zlib/PNG)
 * ==================================================================== */

static void _crcInit(void)
{
    int n;
    int k;
    BsfU32 c;

    for (n = 0; n < 256; n++) {
        c = (BsfU32)n;
        for (k = 0; k < 8; k++)
            c = (c & 1) ? 0xEDB88320UL ^ (c >> 1) : c >> 1;
        _crcTable[n] = c;
    }
    _crcReady = 1;
}

/* Start with crc = 0; chain calls to checksum several buffers. */
static BsfU32 _crcUpdate(BsfU32 crc, const void *data, BsfU32 len)
{
    BsfU32 i;
    const BsfU8 *p = NULL;

    if (!_crcReady)
        _crcInit();

    p = (const BsfU8 *)data;
    crc = crc ^ 0xFFFFFFFFUL;
    for (i = 0; i < len; i++)
        crc = _crcTable[(crc ^ p[i]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}

/* ======================================================================
 * RLE (PackBits style)
 *   control 0..127   : copy the next control+1 bytes literally   (1..128)
 *   control 128..255 : repeat the next byte control-126 times     (2..129)
 * ==================================================================== */

/* Worst-case encoded size for n input bytes. */
static BsfU32 _rleBound(BsfU32 n)
{
    /* Overhead is one header per literal: at most one per 128 bytes plus
     * one per literal cut short by a run (a run consumes >= 3 bytes). */
    return n + n / 2 + 8;
}

/* Returns the encoded size. dst must hold _rleBound(n) bytes. */
static BsfU32 _rleEncode(const BsfU8 *src, BsfU32 n, BsfU8 *dst)
{
    BsfU32 i = 0;
    BsfU32 out = 0;
    BsfU32 run;
    BsfU32 start;
    BsfU32 lit;

    while (i < n) {
        run = 1;
        while (i + run < n
            && run < BSF_RLE_MAX_RUN
            && src[i + run] == src[i])
            run++;

        if (run >= 2) {
            dst[out++] = (BsfU8)(run + 126);
            dst[out++] = src[i];
            i += run;
            continue;
        }

        /* Literal: stop where a run of 3+ starts (a pair is cheaper
         * inline). */
        start = i;
        lit = 0;
        while (i < n && lit < BSF_RLE_MAX_LITERAL) {
            if (i + 2 < n && src[i] == src[i + 1] && src[i] == src[i + 2])
                break;
            i++;
            lit++;
        }
        dst[out++] = (BsfU8)(lit - 1);
        memcpy(dst + out, src + start, lit);
        out += lit;
    }
    return out;
}

/* Returns BSF_OK only if src decodes to exactly dstLen bytes. */
static int _rleDecode(
    const BsfU8 *src,
    BsfU32 srcLen,
    BsfU8 *dst,
    BsfU32 dstLen
)
{
    BsfU32 si = 0;
    BsfU32 di = 0;
    BsfU32 cnt;
    BsfU8 c;

    while (si < srcLen) {
        c = src[si++];
        if (c < 128) {
            cnt = (BsfU32)c + 1;
            if (cnt > srcLen - si || cnt > dstLen - di)
                return BSF_ERR_FORMAT;
            memcpy(dst + di, src + si, cnt);
            si += cnt;
        } else {
            cnt = (BsfU32)c - 126;
            if (si >= srcLen || cnt > dstLen - di)
                return BSF_ERR_FORMAT;
            memset(dst + di, src[si++], cnt);
        }
        di += cnt;
    }
    return di == dstLen ? BSF_OK : BSF_ERR_FORMAT;
}

/* ======================================================================
 * Sequence model
 * ==================================================================== */

const char *bsfStrError(int err)
{
    switch (err) {
    case BSF_OK:              return "ok";
    case BSF_ERR_IO:          return "read/write error or unexpected EOF";
    case BSF_ERR_NOMEM:       return "out of memory";
    case BSF_ERR_SIGNATURE:   return "not a .bsf file";
    case BSF_ERR_VERSION:     return "file was written by a newer version";
    case BSF_ERR_FORMAT:      return "malformed file";
    case BSF_ERR_CRC:         return "checksum mismatch";
    case BSF_ERR_UNSUPPORTED: return "unsupported feature";
    case BSF_ERR_ARG:         return "invalid argument";
    }
    return "unknown error";
}

static int _validBpp(BsfU8 bpp)
{
    return bpp == 1 || bpp == 4 || bpp == 8 || bpp == 16 || bpp == 24;
}

static int _validSize(BsfU16 width, BsfU16 height)
{
    return width >= 1 && width <= BSF_MAX_DIM
        && height >= 1 && height <= BSF_MAX_DIM;
}

BsfU32 bsfRowBytes(BsfU16 width, BsfU8 bpp)
{
    return ((BsfU32)width * bpp + 7) / 8;
}

BsfU32 bsfFrameBytes(const BsfSequence *seq)
{
    return bsfRowBytes(seq->width, seq->bpp) * seq->height;
}

void bsfFree(BsfSequence *seq)
{
    BsfU16 i;

    if (!seq)
        return;
    if (seq->frames) {
        for (i = 0; i < seq->frameCount; i++)
            free(seq->frames[i].pixels);
        free(seq->frames);
    }
    memset(seq, 0, sizeof(*seq));
}

int bsfInit(
    BsfSequence *seq,
    BsfU16 width,
    BsfU16 height,
    BsfU8 bpp,
    BsfU16 frameCount
)
{
    BsfU32 bytes;
    BsfU16 i;

    if (!seq)
        return BSF_ERR_ARG;
    memset(seq, 0, sizeof(*seq));
    if (!_validSize(width, height) || !_validBpp(bpp) || frameCount < 1)
        return BSF_ERR_ARG;

    seq->width = width;
    seq->height = height;
    seq->bpp = bpp;
    seq->frameCount = frameCount;

    seq->frames = (BsfFrame *)calloc(frameCount, sizeof(BsfFrame));
    if (!seq->frames) {
        bsfFree(seq);
        return BSF_ERR_NOMEM;
    }
    bytes = bsfFrameBytes(seq);
    for (i = 0; i < frameCount; i++) {
        seq->frames[i].pixels = (BsfU8 *)calloc(bytes, 1);
        if (!seq->frames[i].pixels) {
            bsfFree(seq);
            return BSF_ERR_NOMEM;
        }
    }
    return BSF_OK;
}

int bsfValidate(const BsfSequence *seq)
{
    BsfU16 i;

    if (!seq)
        return BSF_ERR_ARG;
    if (!_validSize(seq->width, seq->height) || !_validBpp(seq->bpp))
        return BSF_ERR_FORMAT;
    if (seq->bpp <= 8) {
        if (seq->paletteCount < 1)
            return BSF_ERR_FORMAT;
        if (seq->paletteCount > (1u << seq->bpp))
            return BSF_ERR_FORMAT;
        if ((seq->flags & BSF_FLAG_TRANSPARENT)
            && seq->transparent >= seq->paletteCount)
            return BSF_ERR_FORMAT;
    } else {
        if (seq->paletteCount > BSF_PALETTE_MAX)
            return BSF_ERR_FORMAT;
        if ((seq->flags & BSF_FLAG_TRANSPARENT)
            && seq->transparent > 0xFFFFFFUL)
            return BSF_ERR_FORMAT;
    }
    if (memchr(seq->name, '\0', BSF_NAME_LEN + 1) == NULL)
        return BSF_ERR_FORMAT;
    if (seq->frameCount < 1 || !seq->frames)
        return BSF_ERR_FORMAT;
    for (i = 0; i < seq->frameCount; i++) {
        if (!seq->frames[i].pixels)
            return BSF_ERR_FORMAT;
    }
    return BSF_OK;
}

/* ======================================================================
 * Writer
 * ==================================================================== */

/* Writes one chunk whose data is the concatenation of a and b. */
static int _writeChunk(
    FILE *fp,
    const char *id,
    const BsfU8 *a,
    BsfU32 aLen,
    const BsfU8 *b,
    BsfU32 bLen
)
{
    BsfU32 crc;
    BsfU8 hdr[8];
    BsfU8 tail[4];

    memcpy(hdr, id, 4);
    _putU32(hdr + 4, aLen + bLen);
    crc = _crcUpdate(0, id, 4);
    crc = _crcUpdate(crc, a, aLen);
    crc = _crcUpdate(crc, b, bLen);
    _putU32(tail, crc);

    if (!_writeBytes(fp, hdr, 8))
        return BSF_ERR_IO;
    if (!_writeBytes(fp, a, aLen))
        return BSF_ERR_IO;
    if (!_writeBytes(fp, b, bLen))
        return BSF_ERR_IO;
    if (!_writeBytes(fp, tail, 4))
        return BSF_ERR_IO;
    return BSF_OK;
}

static int _writeHead(FILE *fp, const BsfSequence *seq)
{
    BsfU8 head[BSF_HEAD_SIZE];

    _putU16(head + 0, BSF_VERSION);
    _putU16(head + 2, seq->width);
    _putU16(head + 4, seq->height);
    head[6] = seq->bpp;
    head[7] = seq->flags;
    _putU32(head + 8, seq->transparent);
    _putU16(head + 12, seq->frameCount);
    _putU16(head + 14, seq->defaultDelay);
    _putU16(head + 16, seq->loopCount);
    return _writeChunk(
        fp,
        "HEAD",
        head,
        BSF_HEAD_SIZE,
        NULL,
        0
    );
}

static int _writePalette(FILE *fp, const BsfSequence *seq)
{
    BsfU32 len;
    BsfU8 plte[2 + BSF_PALETTE_MAX * 3];

    len = 2 + (BsfU32)seq->paletteCount * 3;
    _putU16(plte, seq->paletteCount);
    memcpy(plte + 2, seq->palette, len - 2);
    return _writeChunk(
        fp,
        "PLTE",
        plte,
        len,
        NULL,
        0
    );
}

static int _writeFrame(
    FILE *fp,
    const BsfFrame *frame,
    BsfU32 rawLen,
    BsfU8 *rle
)
{
    BsfU32 rleLen = rawLen;
    BsfU8 fhdr[BSF_FRAM_HDR];

    _putU16(fhdr, frame->delay);
    fhdr[3] = 0;
    if (rle)
        rleLen = _rleEncode(frame->pixels, rawLen, rle);

    if (rle && rleLen < rawLen) {
        fhdr[2] = BSF_COMP_RLE;
        return _writeChunk(
            fp,
            "FRAM",
            fhdr,
            BSF_FRAM_HDR,
            rle,
            rleLen
        );
    }
    fhdr[2] = BSF_COMP_RAW;
    return _writeChunk(
        fp,
        "FRAM",
        fhdr,
        BSF_FRAM_HDR,
        frame->pixels,
        rawLen
    );
}

int bsfWrite(FILE *fp, const BsfSequence *seq, int compression)
{
    int err;
    BsfU32 rawLen;
    BsfU16 i;
    BsfU8 *rle = NULL;

    if (!fp || !seq)
        return BSF_ERR_ARG;
    if (compression == BSF_COMP_DEFLATE)
        return BSF_ERR_UNSUPPORTED;
    if (compression != BSF_COMP_RAW && compression != BSF_COMP_RLE)
        return BSF_ERR_ARG;
    err = bsfValidate(seq);
    if (err != BSF_OK)
        return err;

    if (!_writeBytes(fp, _signature, 8))
        return BSF_ERR_IO;
    err = _writeHead(fp, seq);
    if (err == BSF_OK && seq->paletteCount > 0)
        err = _writePalette(fp, seq);
    if (err == BSF_OK && seq->name[0]) {
        err = _writeChunk(
            fp,
            "NAME",
            (const BsfU8 *)seq->name,
            (BsfU32)strlen(seq->name),
            NULL,
            0
        );
    }
    if (err != BSF_OK)
        return err;

    rawLen = bsfFrameBytes(seq);
    if (compression == BSF_COMP_RLE) {
        rle = (BsfU8 *)malloc(_rleBound(rawLen));
        if (!rle)
            return BSF_ERR_NOMEM;
    }
    for (i = 0; i < seq->frameCount && err == BSF_OK; i++)
        err = _writeFrame(
            fp,
            &seq->frames[i],
            rawLen,
            rle
        );
    free(rle);
    if (err != BSF_OK)
        return err;

    return _writeChunk(
        fp,
        "END ",
        NULL,
        0,
        NULL,
        0
    );
}

/* ======================================================================
 * Reader
 * ==================================================================== */

/* Reads len data bytes plus the CRC into a new buffer; checks the CRC. */
static int _readChunk(
    FILE *fp,
    const BsfU8 *id,
    BsfU32 len,
    unsigned int options,
    BsfU8 **out
)
{
    BsfU32 crc;
    BsfU8 tail[4];
    BsfU8 *buf = NULL;

    *out = NULL;
    buf = (BsfU8 *)malloc(len ? len : 1);
    if (!buf)
        return BSF_ERR_NOMEM;
    if (!_readBytes(fp, buf, len) || !_readBytes(fp, tail, 4)) {
        free(buf);
        return BSF_ERR_IO;
    }
    if (!(options & BSF_READ_NO_CRC)) {
        crc = _crcUpdate(_crcUpdate(0, id, 4), buf, len);
        if (crc != _getU32(tail)) {
            free(buf);
            return BSF_ERR_CRC;
        }
    }
    *out = buf;
    return BSF_OK;
}

static int _parseHead(BsfSequence *seq, const BsfU8 *d)
{
    if (_getU16(d) > BSF_VERSION)
        return BSF_ERR_VERSION;
    if (_getU16(d) < 1)
        return BSF_ERR_FORMAT;

    seq->width = _getU16(d + 2);
    seq->height = _getU16(d + 4);
    seq->bpp = d[6];
    seq->flags = d[7];
    seq->transparent = _getU32(d + 8);
    seq->frameCount = _getU16(d + 12);
    seq->defaultDelay = _getU16(d + 14);
    seq->loopCount = _getU16(d + 16);

    if (!_validSize(seq->width, seq->height) || !_validBpp(seq->bpp))
        return BSF_ERR_FORMAT;
    if (seq->frameCount < 1)
        return BSF_ERR_FORMAT;

    seq->frames = (BsfFrame *)calloc(seq->frameCount, sizeof(BsfFrame));
    if (!seq->frames)
        return BSF_ERR_NOMEM;
    return BSF_OK;
}

static int _parsePalette(BsfSequence *seq, const BsfU8 *d, BsfU32 len)
{
    BsfU32 n;

    n = _getU16(d);
    if (n > BSF_PALETTE_MAX || len != 2 + n * 3)
        return BSF_ERR_FORMAT;
    seq->paletteCount = (BsfU16)n;
    memcpy(seq->palette, d + 2, n * 3);
    return BSF_OK;
}

static void _parseName(BsfSequence *seq, const BsfU8 *d, BsfU32 len)
{
    BsfU32 n;

    n = len < BSF_NAME_LEN ? len : BSF_NAME_LEN;
    memcpy(seq->name, d, n);
    seq->name[n] = '\0';
}

static int _parseFrame(
    const BsfSequence *seq,
    BsfFrame *frame,
    const BsfU8 *d,
    BsfU32 len
)
{
    BsfU32 rawLen;
    BsfU8 comp;

    if (len < BSF_FRAM_HDR)
        return BSF_ERR_FORMAT;
    rawLen = bsfFrameBytes(seq);
    frame->delay = _getU16(d);
    comp = d[2];
    d += BSF_FRAM_HDR;
    len -= BSF_FRAM_HDR;

    if (comp == BSF_COMP_DEFLATE)
        return BSF_ERR_UNSUPPORTED;
    if (comp != BSF_COMP_RAW && comp != BSF_COMP_RLE)
        return BSF_ERR_FORMAT;
    if (comp == BSF_COMP_RAW && len != rawLen)
        return BSF_ERR_FORMAT;

    frame->pixels = (BsfU8 *)malloc(rawLen);
    if (!frame->pixels)
        return BSF_ERR_NOMEM;
    if (comp == BSF_COMP_RAW) {
        memcpy(frame->pixels, d, rawLen);
        return BSF_OK;
    }
    return _rleDecode(
        d,
        len,
        frame->pixels,
        rawLen
    );
}

/* Skips data + CRC of a chunk this version does not know. */
static int _skipChunk(FILE *fp, BsfU32 len)
{
    if (len > (BsfU32)LONG_MAX - 4)
        return BSF_ERR_IO;
    if (fseek(fp, (long)len + 4, SEEK_CUR) != 0)
        return BSF_ERR_IO;
    return BSF_OK;
}

/* Upper bound for each known chunk's data, to reject absurd lengths
 * before allocating. Returns 0 for "invalid here". */
static BsfU32 _chunkLimit(const BsfU8 *id, const BsfSequence *seq)
{
    if (memcmp(id, "PLTE", 4) == 0)
        return 2 + BSF_PALETTE_MAX * 3;
    if (memcmp(id, "NAME", 4) == 0)
        return BSF_NAME_MAX_READ;
    if (memcmp(id, "FRAM", 4) == 0)
        return BSF_FRAM_HDR + _rleBound(bsfFrameBytes(seq));
    return 0;
}

int bsfRead(FILE *fp, BsfSequence *seq, unsigned int options)
{
    int err = BSF_OK;
    int havePalette = 0;
    BsfU32 len;
    BsfU16 framesRead = 0;
    BsfU8 sig[8];
    BsfU8 hdr[8];
    BsfU8 *data = NULL;

    if (!fp || !seq)
        return BSF_ERR_ARG;
    memset(seq, 0, sizeof(*seq));

    if (!_readBytes(fp, sig, 8))
        return BSF_ERR_IO;
    if (memcmp(sig, _signature, 8) != 0)
        return BSF_ERR_SIGNATURE;

    /* HEAD must come first. */
    if (!_readBytes(fp, hdr, 8))
        return BSF_ERR_IO;
    len = _getU32(hdr + 4);
    if (memcmp(hdr, "HEAD", 4) != 0)
        return BSF_ERR_FORMAT;
    if (len < BSF_HEAD_SIZE || len > BSF_HEAD_MAX)
        return BSF_ERR_FORMAT;
    err = _readChunk(
        fp,
        hdr,
        len,
        options,
        &data
    );
    if (err == BSF_OK)
        err = _parseHead(seq, data);
    free(data);
    data = NULL;

    while (err == BSF_OK) {
        if (!_readBytes(fp, hdr, 8)) {
            err = BSF_ERR_IO;
            break;
        }
        len = _getU32(hdr + 4);

        if (memcmp(hdr, "END ", 4) == 0) {
            if (len != 0) {
                err = BSF_ERR_FORMAT;
                break;
            }
            err = _readChunk(
                fp,
                hdr,
                0,
                options,
                &data
            );
            break;
        }

        if (_chunkLimit(hdr, seq) == 0) {
            err = _skipChunk(fp, len);
            continue;
        }
        if (len > _chunkLimit(hdr, seq)) {
            err = BSF_ERR_FORMAT;
            break;
        }
        if (memcmp(hdr, "PLTE", 4) == 0 && havePalette) {
            err = BSF_ERR_FORMAT;
            break;
        }
        if (memcmp(hdr, "FRAM", 4) == 0 && framesRead >= seq->frameCount) {
            err = BSF_ERR_FORMAT;
            break;
        }

        err = _readChunk(
            fp,
            hdr,
            len,
            options,
            &data
        );
        if (err != BSF_OK)
            break;

        if (memcmp(hdr, "PLTE", 4) == 0) {
            err = _parsePalette(seq, data, len);
            havePalette = 1;
        } else if (memcmp(hdr, "NAME", 4) == 0) {
            _parseName(seq, data, len);
        } else {
            err = _parseFrame(
                seq,
                &seq->frames[framesRead],
                data,
                len
            );
            framesRead++;
        }
        free(data);
        data = NULL;
    }
    free(data);

    if (err == BSF_OK && framesRead != seq->frameCount)
        err = BSF_ERR_FORMAT;
    if (err == BSF_OK)
        err = bsfValidate(seq);
    if (err != BSF_OK)
        bsfFree(seq);
    return err;
}

/* ======================================================================
 * Tests: build with -DSTANDALONE
 * ==================================================================== */
#ifdef STANDALONE

static int _failures = 0;
static int _checks = 0;
static unsigned long _rngState = 12345;

#define BSF_CHECK(cond) do { \
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

#define BSF_CHECK_ERR(expr, want) do { \
    int got_ = (expr); \
    _checks++; \
    if (got_ != (want)) { \
        _failures++; \
        printf( \
            "FAIL %s:%d: %s = %d (%s), want %d\n", \
            __FILE__, \
            __LINE__, \
            #expr, \
            got_, \
            bsfStrError(got_), \
            (want) \
        ); \
    } \
} while (0)

static BsfU8 _rnd(void)
{
    _rngState = _rngState * 1103515245UL + 12345UL;
    return (BsfU8)((_rngState >> 16) & 0xFF);
}

static FILE *_tmpFile(void)
{
    FILE *fp = NULL;

    fp = tmpfile();
    if (!fp) {
        printf("cannot create temp file\n");
        exit(2);
    }
    return fp;
}

/* Loads a whole file into memory (caller frees). */
static BsfU8 *_slurp(FILE *fp, long *size)
{
    BsfU8 *buf = NULL;

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    buf = (BsfU8 *)malloc((size_t)*size);
    rewind(fp);
    if (!buf || !_readBytes(fp, buf, (BsfU32)*size)) {
        printf("slurp failed\n");
        exit(2);
    }
    return buf;
}

static FILE *_fileFrom(const BsfU8 *data, long size)
{
    FILE *fp = NULL;

    fp = _tmpFile();
    _writeBytes(fp, data, (BsfU32)size);
    rewind(fp);
    return fp;
}

/* pattern 0 = noise, 1 = long runs */
static void _makeSeq(
    BsfSequence *s,
    BsfU16 w,
    BsfU16 h,
    BsfU8 bpp,
    BsfU16 frames,
    int pattern
)
{
    BsfU32 bytes;
    BsfU32 i;
    BsfU16 f;

    BSF_CHECK_ERR(
        bsfInit(
            s,
            w,
            h,
            bpp,
            frames
        ),
        BSF_OK
    );
    if (bpp <= 8) {
        s->paletteCount = (BsfU16)(1u << bpp);
        for (i = 0; i < (BsfU32)s->paletteCount * 3; i++)
            s->palette[i] = (BsfU8)(i * 7);
        s->flags = BSF_FLAG_TRANSPARENT | BSF_FLAG_LOOP;
        s->transparent = (BsfU32)s->paletteCount - 1;
    } else {
        s->flags = BSF_FLAG_TRANSPARENT;
        s->transparent = 0xFF00FFUL;
    }
    s->defaultDelay = 100;
    s->loopCount = 3;
    strcpy(s->name, "walk_left");

    bytes = bsfFrameBytes(s);
    for (f = 0; f < frames; f++) {
        s->frames[f].delay = (BsfU16)(f * 10);
        for (i = 0; i < bytes; i++) {
            if (pattern == 0)
                s->frames[f].pixels[i] = _rnd();
            else
                s->frames[f].pixels[i] = (BsfU8)((i / 37 + f) & 0xFF);
        }
    }
}

static int _seqEqual(const BsfSequence *a, const BsfSequence *b)
{
    BsfU32 bytes;
    BsfU16 f;

    if (a->width != b->width || a->height != b->height)
        return 0;
    if (a->bpp != b->bpp || a->flags != b->flags)
        return 0;
    if (a->transparent != b->transparent)
        return 0;
    if (a->defaultDelay != b->defaultDelay)
        return 0;
    if (a->loopCount != b->loopCount)
        return 0;
    if (a->paletteCount != b->paletteCount)
        return 0;
    if (a->frameCount != b->frameCount)
        return 0;
    if (strcmp(a->name, b->name) != 0)
        return 0;
    if (memcmp(a->palette, b->palette, (size_t)a->paletteCount * 3) != 0)
        return 0;

    bytes = bsfFrameBytes(a);
    for (f = 0; f < a->frameCount; f++) {
        if (a->frames[f].delay != b->frames[f].delay)
            return 0;
        if (memcmp(a->frames[f].pixels, b->frames[f].pixels, bytes) != 0)
            return 0;
    }
    return 1;
}

static void _testCrc(void)
{
    BSF_CHECK(_crcUpdate(0, "123456789", 9) == 0xCBF43926UL);
    BSF_CHECK(_crcUpdate(0, "", 0) == 0);
}

static void _rleRoundtrip(const BsfU8 *src, BsfU32 n)
{
    BsfU32 len;
    BsfU8 *enc = NULL;
    BsfU8 *dec = NULL;

    enc = (BsfU8 *)malloc(_rleBound(n));
    dec = (BsfU8 *)malloc(n ? n : 1);
    len = _rleEncode(src, n, enc);
    BSF_CHECK(len <= _rleBound(n));
    BSF_CHECK_ERR(
        _rleDecode(
            enc,
            len,
            dec,
            n
        ),
        BSF_OK
    );
    BSF_CHECK(n == 0 || memcmp(src, dec, n) == 0);
    free(enc);
    free(dec);
}

/* Decodes into a scratch buffer of dstLen (<= 64) bytes. */
static int _rleTryDecode(const BsfU8 *src, BsfU32 srcLen, BsfU32 dstLen)
{
    BsfU8 out[64];

    return _rleDecode(
        src,
        srcLen,
        out,
        dstLen
    );
}

static void _testRle(void)
{
    BsfU32 i;
    BsfU8 buf[1000];
    BsfU8 enc[16];

    memset(buf, 0, sizeof(buf));
    _rleRoundtrip(buf, 0);
    buf[0] = 5;
    _rleRoundtrip(buf, 1);

    memset(buf, 9, sizeof(buf));                /* long run, > 129 */
    _rleRoundtrip(buf, sizeof(buf));
    BSF_CHECK(_rleEncode(buf, sizeof(buf), enc) < 20);

    for (i = 0; i < sizeof(buf); i++)           /* no runs, > 128 */
        buf[i] = (BsfU8)i;
    _rleRoundtrip(buf, sizeof(buf));

    for (i = 0; i < sizeof(buf); i++)           /* a bb a bb ... */
        buf[i] = (BsfU8)((i % 3 == 0) ? 1 : 2 + (i / 3) % 2);
    _rleRoundtrip(buf, sizeof(buf));

    for (i = 0; i < sizeof(buf); i++)
        buf[i] = (BsfU8)(_rnd() & 3);
    _rleRoundtrip(buf, sizeof(buf));

    /* Malformed streams */
    enc[0] = 3;                                 /* literal of 4, 1 byte */
    enc[1] = 1;
    BSF_CHECK_ERR(_rleTryDecode(enc, 2, 4), BSF_ERR_FORMAT);
    enc[0] = 200;                               /* run of 74 into 8 */
    enc[1] = 7;
    BSF_CHECK_ERR(_rleTryDecode(enc, 2, 8), BSF_ERR_FORMAT);
    enc[0] = 128 + 2;                           /* run of 4 into 8 */
    BSF_CHECK_ERR(_rleTryDecode(enc, 2, 8), BSF_ERR_FORMAT);
    enc[0] = 200;                               /* run without value */
    BSF_CHECK_ERR(_rleTryDecode(enc, 1, 8), BSF_ERR_FORMAT);
}

static void _roundtrip(
    BsfU16 w,
    BsfU16 h,
    BsfU8 bpp,
    BsfU16 frames,
    int pattern,
    int comp
)
{
    BsfSequence a;
    BsfSequence b;
    FILE *fp = NULL;

    fp = _tmpFile();
    _makeSeq(
        &a,
        w,
        h,
        bpp,
        frames,
        pattern
    );
    BSF_CHECK_ERR(bsfWrite(fp, &a, comp), BSF_OK);
    rewind(fp);
    BSF_CHECK_ERR(bsfRead(fp, &b, 0), BSF_OK);
    BSF_CHECK(_seqEqual(&a, &b));
    bsfFree(&a);
    bsfFree(&b);
    fclose(fp);
}

static void _testRoundtrip(void)
{
    int d;
    int pattern;
    int comp;
    static const BsfU8 depths[] = { 1, 4, 8, 16, 24 };

    for (d = 0; d < 5; d++) {
        for (pattern = 0; pattern < 2; pattern++) {
            for (comp = BSF_COMP_RAW; comp <= BSF_COMP_RLE; comp++) {
                /* odd width exercises row packing */
                _roundtrip(
                    13,
                    7,
                    depths[d],
                    3,
                    pattern,
                    comp
                );
                _roundtrip(
                    1,
                    1,
                    depths[d],
                    1,
                    pattern,
                    comp
                );
            }
        }
    }
    _roundtrip(
        320,
        200,
        8,
        4,
        1,
        BSF_COMP_RLE
    );
}

static void _testRleSmaller(void)
{
    BsfSequence a;
    FILE *raw = NULL;
    FILE *rle = NULL;

    raw = _tmpFile();
    rle = _tmpFile();
    _makeSeq(
        &a,
        64,
        64,
        8,
        2,
        1
    );
    bsfWrite(raw, &a, BSF_COMP_RAW);
    bsfWrite(rle, &a, BSF_COMP_RLE);
    BSF_CHECK(ftell(rle) < ftell(raw) / 4);
    bsfFree(&a);
    fclose(raw);
    fclose(rle);
}

static void _testValidation(void)
{
    BsfSequence a;
    FILE *fp = NULL;

    fp = _tmpFile();
    _makeSeq(
        &a,
        4,
        4,
        8,
        1,
        1
    );
    a.paletteCount = 0;                         /* indexed, no palette */
    BSF_CHECK_ERR(bsfWrite(fp, &a, BSF_COMP_RAW), BSF_ERR_FORMAT);
    a.paletteCount = 256;
    a.transparent = 256;                        /* outside palette */
    BSF_CHECK_ERR(bsfWrite(fp, &a, BSF_COMP_RAW), BSF_ERR_FORMAT);
    a.transparent = 255;
    BSF_CHECK_ERR(bsfWrite(fp, &a, BSF_COMP_DEFLATE), BSF_ERR_UNSUPPORTED);
    BSF_CHECK_ERR(bsfWrite(fp, &a, 99), BSF_ERR_ARG);
    bsfFree(&a);

    BSF_CHECK_ERR(
        bsfInit(
            &a,
            0,
            4,
            8,
            1
        ),
        BSF_ERR_ARG
    );
    BSF_CHECK_ERR(
        bsfInit(
            &a,
            4,
            4,
            3,
            1
        ),
        BSF_ERR_ARG
    );
    BSF_CHECK_ERR(
        bsfInit(
            &a,
            BSF_MAX_DIM + 1,
            4,
            8,
            1
        ),
        BSF_ERR_ARG
    );
    fclose(fp);
}

/* Writes a valid file and returns its bytes for corruption tests. */
static BsfU8 *_validFile(long *size)
{
    BsfSequence a;
    FILE *fp = NULL;
    BsfU8 *buf = NULL;

    fp = _tmpFile();
    _makeSeq(
        &a,
        8,
        8,
        8,
        2,
        1
    );
    bsfWrite(fp, &a, BSF_COMP_RLE);
    buf = _slurp(fp, size);
    bsfFree(&a);
    fclose(fp);
    return buf;
}

static int _readFrom(const BsfU8 *data, long size, unsigned int options)
{
    int err;
    BsfSequence s;
    FILE *fp = NULL;

    fp = _fileFrom(data, size);
    err = bsfRead(fp, &s, options);
    if (err != BSF_OK)                          /* left empty on error */
        BSF_CHECK(s.frames == NULL && s.frameCount == 0);
    bsfFree(&s);
    fclose(fp);
    return err;
}

static void _testCorrupt(void)
{
    long size;
    long i;
    BsfU8 *good = NULL;
    BsfU8 *bad = NULL;

    good = _validFile(&size);
    bad = (BsfU8 *)malloc((size_t)size);

    BSF_CHECK_ERR(_readFrom(good, size, 0), BSF_OK);

    memcpy(bad, good, (size_t)size);
    bad[5] = 'X';                               /* signature */
    BSF_CHECK_ERR(_readFrom(bad, size, 0), BSF_ERR_SIGNATURE);

    memcpy(bad, good, (size_t)size);
    bad[8 + 8 + 6] ^= 0x01;                     /* HEAD data byte (bpp) */
    BSF_CHECK_ERR(_readFrom(bad, size, 0), BSF_ERR_CRC);

    memcpy(bad, good, (size_t)size);
    bad[8 + 8] = 2;                             /* version 2 */
    BSF_CHECK_ERR(_readFrom(bad, size, BSF_READ_NO_CRC), BSF_ERR_VERSION);

    /* Every truncation must fail cleanly */
    for (i = 0; i < size; i++)
        BSF_CHECK(_readFrom(good, i, 0) != BSF_OK);

    /* Flipping any single byte must never crash */
    for (i = 0; i < size; i++) {
        memcpy(bad, good, (size_t)size);
        bad[i] ^= 0x5A;
        _readFrom(bad, size, 0);
        _readFrom(bad, size, BSF_READ_NO_CRC);
    }

    free(bad);
    free(good);
}

static void _testUnknownChunk(void)
{
    long size;
    long headEnd = 8 + 8 + BSF_HEAD_SIZE + 4;   /* signature + HEAD */
    BsfU8 extra[16] = {
        'X', 'T', 'R', 'A', 4, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0, 0
    };
    BsfU8 *good = NULL;
    BsfU8 *patched = NULL;

    /* An unknown chunk after HEAD must be skipped. */
    good = _validFile(&size);
    patched = (BsfU8 *)malloc((size_t)size + 16);
    memcpy(patched, good, (size_t)headEnd);
    memcpy(patched + headEnd, extra, 16);
    memcpy(
        patched + headEnd + 16,
        good + headEnd,
        (size_t)(size - headEnd)
    );
    BSF_CHECK_ERR(_readFrom(patched, size + 16, 0), BSF_OK);

    free(patched);
    free(good);
}

static void _testEmbedded(void)
{
    BsfSequence a;
    BsfSequence b;
    FILE *fp = NULL;

    /* A .bsf can be read from the middle of a bigger file (.bss/.bpk). */
    fp = _tmpFile();
    _makeSeq(
        &a,
        16,
        16,
        4,
        2,
        0
    );
    _writeBytes(fp, "PREFIX", 6);
    bsfWrite(fp, &a, BSF_COMP_RLE);
    _writeBytes(fp, "SUFFIX", 6);
    fseek(fp, 6, SEEK_SET);
    BSF_CHECK_ERR(bsfRead(fp, &b, 0), BSF_OK);
    BSF_CHECK(_seqEqual(&a, &b));
    bsfFree(&a);
    bsfFree(&b);
    fclose(fp);
}

int main(void)
{
    _testCrc();
    _testRle();
    _testRoundtrip();
    _testRleSmaller();
    _testValidation();
    _testCorrupt();
    _testUnknownChunk();
    _testEmbedded();

    printf("bsf: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}

#endif /* STANDALONE */
