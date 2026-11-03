/*
 * font - bitmap font and icons for the UI (8-bit alpha atlas).
 *
 * Proportional ASCII font. Bytes outside 32..126 are drawn as glyph 127;
 * UTF-8 continuation bytes are skipped, so each character takes one glyph.
 */
#ifndef FONT_H
#define FONT_H

#include "../core/raster.h"

#define FONT_HEIGHT     18

/* Icon ids, same values as microui's MU_ICON_* */
enum {
    FONT_ICON_CLOSE = 1,
    FONT_ICON_CHECK,
    FONT_ICON_COLLAPSED,
    FONT_ICON_EXPANDED
};

/* len < 0: up to the terminating '\0'. */
int font_textWidth(const char *str, int len);

/* Mask for one character / icon, ready for ras_blitMask(). */
void font_glyph(int chr, RasMask *mask);
int font_icon(int id, RasMask *mask);       /* 0 if id is unknown */

/* Draws text with its top-left corner at (x, y). */
void font_drawText(
    RasFb *fb,
    RasRect clip,
    const char *str,
    int len,
    int x,
    int y,
    TkU32 argb
);

#endif /* FONT_H */
