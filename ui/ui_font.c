/*
 * ui_font - bitmap font and icons (module: ui).
 */
#include "private.h"

/* ---- Prototypes of file-private functions ---- */
static int _ui_isContinuation(char c);
static void _ui_setMask(RasMask *mask, const int *rect);

/* ================================================================
 * 2. Module internal (declared in private.h)
 * ================================================================ */

/*
 * Measures the width of a text in pixels.
 * Expects: a string and its length in bytes (< 0: up to the '\0').
 * Returns: the width.
 */
int _ui_textWidth(const char *str, int len)
{
    int i;
    int width = 0;
    RasMask mask;

    if (len < 0)
        len = (int)strlen(str);
    for (i = 0; i < len; i++) {
        if (_ui_isContinuation(str[i]))
            continue;
        _ui_glyph((unsigned char)str[i], &mask);
        width += mask.src.w;
    }
    return width;
}

/*
 * Draws a text with its top-left corner at (x, y).
 * Expects: a framebuffer, the clip, a '\0'-terminated string, a position
 *          and a 0xAARRGGBB color.
 * Returns: nothing.
 */
void _ui_drawText(
    RasFb *fb,
    RasRect clip,
    const char *str,
    int x,
    int y,
    TkU32 argb
)
{
    RasMask mask;
    const char *p = NULL;

    for (p = str; *p; p++) {
        if (_ui_isContinuation(*p))
            continue;
        _ui_glyph((unsigned char)*p, &mask);
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

/*
 * Gets the atlas region of a character.
 * Expects: a character code; codes outside 32..127 map to 127.
 * Returns: nothing (fills mask).
 */
void _ui_glyph(int chr, RasMask *mask)
{
    if (chr < UI_FONT_FIRST || chr > UI_FONT_LAST)
        chr = UI_FONT_LAST;
    _ui_setMask(mask, _ui_fontGlyphs[chr - UI_FONT_FIRST]);
}

/*
 * Gets the atlas region of an icon.
 * Expects: a UI_ICON_* id (same values as MU_ICON_*).
 * Returns: 1 and fills mask, or 0 if the id is unknown.
 */
int _ui_icon(int id, RasMask *mask)
{
    if (id < UI_ICON_CLOSE || id > UI_ICON_EXPANDED)
        return 0;
    _ui_setMask(mask, _ui_fontIcons[id - UI_ICON_CLOSE]);
    return 1;
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * Tells whether a byte continues a UTF-8 character.
 * Expects: any byte.
 * Returns: 1 for 10xxxxxx bytes, 0 otherwise.
 */
static int _ui_isContinuation(char c)
{
    return ((unsigned char)c & 0xC0) == 0x80;
}

/*
 * Points a mask at a region of the font atlas.
 * Expects: a mask and a { x, y, w, h } region.
 * Returns: nothing.
 */
static void _ui_setMask(RasMask *mask, const int *rect)
{
    mask->alpha = _ui_fontTexture;
    mask->stride = UI_FONT_TEX_W;
    mask->src.x = rect[0];
    mask->src.y = rect[1];
    mask->src.w = rect[2];
    mask->src.h = rect[3];
}

#ifdef STANDALONE
/* ================================================================
 * 4. Tests: built with -DSTANDALONE as a test program
 * ================================================================ */

static int _failures = 0;
static int _checks = 0;

#define UI_CHECK(cond) do { \
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
 * Runs the font tests.
 * Expects: nothing.
 * Returns: 0 when every check passes, 1 otherwise.
 */
int main(void)
{
    RasMask a;
    RasMask b;

    _ui_glyph('a', &a);
    _ui_glyph('b', &b);
    UI_CHECK(a.src.w > 0 && a.src.h > 0);
    UI_CHECK(_ui_textWidth("ab", -1) == a.src.w + b.src.w);
    UI_CHECK(_ui_textWidth("ab", 1) == a.src.w);
    /* a UTF-8 character takes one glyph */
    UI_CHECK(_ui_textWidth("\xC3\xA1", -1) == _ui_textWidth("\x7F", -1));
    UI_CHECK(_ui_icon(UI_ICON_CHECK, &a) == 1);
    UI_CHECK(_ui_icon(99, &a) == 0);

    printf("ui_font: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}

#endif /* STANDALONE */
