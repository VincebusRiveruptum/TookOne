/*
 * img_base - image lifecycle and error messages (module: image).
 */
#include "image.h"

/* ---- Prototypes of file-private functions ---- */
static int _img_validBpp(int bpp);

/* ================================================================
 * 1. Public API
 * ================================================================ */

/*
 * Creates an image with zero-filled pixels and an empty palette.
 * Expects: img to fill, size 1..IMG_MAX_DIM and bpp 1/4/8/16/24.
 * Returns: IMG_OK, IMG_ERR_ARG or IMG_ERR_NOMEM (img is left empty).
 */
int img_create(
    Img *img,
    int width,
    int height,
    int bpp
)
{
    size_t count;

    if (!img)
        return IMG_ERR_ARG;
    memset(img, 0, sizeof(*img));
    if (width < 1 || width > IMG_MAX_DIM)
        return IMG_ERR_ARG;
    if (height < 1 || height > IMG_MAX_DIM)
        return IMG_ERR_ARG;
    if (!_img_validBpp(bpp))
        return IMG_ERR_ARG;

    img->width = width;
    img->height = height;
    img->bpp = bpp;
    count = (size_t)width * (size_t)height;

    if (img_isIndexed(img))
        img->index = (TkU8 *)calloc(count, sizeof(TkU8));
    else
        img->rgb = (TkU32 *)calloc(count, sizeof(TkU32));

    if (!img->index && !img->rgb) {
        img_free(img);
        return IMG_ERR_NOMEM;
    }
    return IMG_OK;
}

/*
 * Releases the pixels of an image and leaves it empty.
 * Expects: an image created by img_create / a codec, or an empty one.
 * Returns: nothing.
 */
void img_free(Img *img)
{
    if (!img)
        return;
    free(img->index);
    free(img->rgb);
    memset(img, 0, sizeof(*img));
}

/*
 * Tells whether the image uses palette indices.
 * Expects: a valid image.
 * Returns: 1 for 1/4/8 bpp, 0 for 16/24 bpp.
 */
int img_isIndexed(const Img *img)
{
    return img->bpp <= 8;
}

/*
 * Describes an IMG_* error code.
 * Expects: a value returned by an img_* function.
 * Returns: a static, human-readable message.
 */
const char *img_strError(int err)
{
    switch (err) {
    case IMG_OK:              return "ok";
    case IMG_ERR_ARG:         return "invalid argument";
    case IMG_ERR_NOMEM:       return "out of memory";
    case IMG_ERR_IO:          return "cannot read file";
    case IMG_ERR_SIGNATURE:   return "unknown file format";
    case IMG_ERR_FORMAT:      return "malformed file";
    case IMG_ERR_UNSUPPORTED: return "unsupported format variant";
    }
    return "unknown error";
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * Tells whether bpp is a supported color depth.
 * Expects: any value.
 * Returns: 1 for 1, 4, 8, 16 or 24; 0 otherwise.
 */
static int _img_validBpp(int bpp)
{
    return bpp == 1 || bpp == 4 || bpp == 8 || bpp == 16 || bpp == 24;
}

#ifdef STANDALONE
/* ================================================================
 * 4. Tests: built with -DSTANDALONE as a test program
 * ================================================================ */

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

/*
 * Runs the image lifecycle tests.
 * Expects: nothing.
 * Returns: 0 when every check passes, 1 otherwise.
 */
int main(void)
{
    Img img;

    IMG_CHECK(img_create(
        &img,
        4,
        3,
        8
    ) == IMG_OK);
    IMG_CHECK(img.index != NULL && img.rgb == NULL);
    IMG_CHECK(img_isIndexed(&img));
    IMG_CHECK(img.index[11] == 0);
    img_free(&img);
    IMG_CHECK(img.index == NULL && img.width == 0);

    IMG_CHECK(img_create(
        &img,
        2,
        2,
        24
    ) == IMG_OK);
    IMG_CHECK(img.rgb != NULL && !img_isIndexed(&img));
    img_free(&img);

    IMG_CHECK(img_create(
        &img,
        0,
        2,
        8
    ) == IMG_ERR_ARG);
    IMG_CHECK(img_create(
        &img,
        2,
        IMG_MAX_DIM + 1,
        8
    ) == IMG_ERR_ARG);
    IMG_CHECK(img_create(
        &img,
        2,
        2,
        3
    ) == IMG_ERR_ARG);
    IMG_CHECK(img_create(
        NULL,
        2,
        2,
        8
    ) == IMG_ERR_ARG);
    IMG_CHECK(strcmp(img_strError(IMG_ERR_NOMEM), "out of memory") == 0);
    IMG_CHECK(strcmp(img_strError(99), "unknown error") == 0);

    printf("img_base: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}

#endif /* STANDALONE */
