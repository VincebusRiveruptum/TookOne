#include <string.h>
#include "font.h"
#include "fontdata.h"

#define FONT_FIRST      32
#define FONT_LAST       127

static int _isContinuation(char c)
{
    return ((unsigned char)c & 0xC0) == 0x80;
}

static void _setMask(RasMask *mask, const int *rect)
{
    mask->alpha = _fontTexture;
    mask->stride = FONT_TEX_W;
    mask->src.x = rect[0];
    mask->src.y = rect[1];
    mask->src.w = rect[2];
    mask->src.h = rect[3];
}

void font_glyph(int chr, RasMask *mask)
{
    if (chr < FONT_FIRST || chr > FONT_LAST)
        chr = FONT_LAST;
    _setMask(mask, _fontGlyphs[chr - FONT_FIRST]);
}

int font_icon(int id, RasMask *mask)
{
    if (id < FONT_ICON_CLOSE || id > FONT_ICON_EXPANDED)
        return 0;
    _setMask(mask, _fontIcons[id - FONT_ICON_CLOSE]);
    return 1;
}

int font_textWidth(const char *str, int len)
{
    int i;
    int width = 0;
    RasMask mask;

    if (len < 0)
        len = (int)strlen(str);
    for (i = 0; i < len; i++) {
        if (_isContinuation(str[i]))
            continue;
        font_glyph((unsigned char)str[i], &mask);
        width += mask.src.w;
    }
    return width;
}

void font_drawText(
    RasFb *fb,
    RasRect clip,
    const char *str,
    int len,
    int x,
    int y,
    TkU32 argb
)
{
    int i;
    RasMask mask;

    if (len < 0)
        len = (int)strlen(str);
    for (i = 0; i < len; i++) {
        if (_isContinuation(str[i]))
            continue;
        font_glyph((unsigned char)str[i], &mask);
        ras_blitMask(
            fb,
            clip,
            &mask,
            x,
            y,
            argb
        );
        x += mask.src.w;
    }
}
