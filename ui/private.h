/*
 * ui/private.h - internals of the ui module.
 * Included only by the module's own .c files.
 */
#ifndef UI_PRIVATE_H
#define UI_PRIVATE_H

#include "ui.h"

/* ---- Font (ui_font.c, data in ui_fdata.c) ----
 * Proportional ASCII font. Bytes outside 32..126 are drawn as glyph 127;
 * UTF-8 continuation bytes are skipped, so each character takes one glyph.
 */

#define UI_FONT_HEIGHT      18
#define UI_FONT_FIRST       32
#define UI_FONT_LAST        127
#define UI_FONT_GLYPHS      (UI_FONT_LAST - UI_FONT_FIRST + 1)
#define UI_FONT_ICONS       4
#define UI_FONT_TEX_W       128
#define UI_FONT_TEX_H       128

/* Icon ids, same values as microui's MU_ICON_* */
enum {
    UI_ICON_CLOSE = 1,
    UI_ICON_CHECK,
    UI_ICON_COLLAPSED,
    UI_ICON_EXPANDED
};

extern const TkU8 _ui_fontTexture[UI_FONT_TEX_W * UI_FONT_TEX_H];
extern const int _ui_fontGlyphs[UI_FONT_GLYPHS][4];     /* x, y, w, h */
extern const int _ui_fontIcons[UI_FONT_ICONS][4];       /* x, y, w, h */

int _ui_textWidth(const char *str, int len);
void _ui_glyph(int chr, RasMask *mask);
int _ui_icon(int id, RasMask *mask);
void _ui_drawText(
    RasFb *fb,
    RasRect clip,
    const char *str,
    int x,
    int y,
    TkU32 argb
);

#endif /* UI_PRIVATE_H */
