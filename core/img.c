#include <stdlib.h>
#include <string.h>
#include "img.h"

static int _validBpp(int bpp)
{
    return bpp == 1 || bpp == 4 || bpp == 8 || bpp == 16 || bpp == 24;
}

int img_isIndexed(const Img *img)
{
    return img->bpp <= 8;
}

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
    if (!_validBpp(bpp))
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

void img_free(Img *img)
{
    if (!img)
        return;
    free(img->index);
    free(img->rgb);
    memset(img, 0, sizeof(*img));
}
