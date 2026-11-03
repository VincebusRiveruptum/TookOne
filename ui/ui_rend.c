/*
 * ui_rend - microui software renderer and the image control (module: ui).
 */
#include "private.h"

/* Custom command types start after microui's own. */
#define UI_COMMAND_IMAGE    (MU_COMMAND_MAX + 1)

/* microui's "no clipping" rectangle size (unclipped_rect in microui.c) */
#define UI_UNCLIPPED        0x1000000

typedef struct UiImageCommand {
    mu_BaseCommand base;
    mu_Rect rect;
    RasImage image;
} UiImageCommand;

/* ---- Prototypes of file-private functions ---- */
static int _ui_textWidthCb(mu_Font font, const char *str, int len);
static int _ui_textHeightCb(mu_Font font);
static void _ui_drawIcon(RasFb *fb, RasRect clip, const mu_IconCommand *cmd);
static TkU32 _ui_argb(mu_Color c);
static RasRect _ui_rect(mu_Rect r);

/* ================================================================
 * 1. Public API
 * ================================================================ */

/*
 * Connects microui to the UI font for text measuring.
 * Expects: a context already initialized with mu_init().
 * Returns: nothing.
 */
void ui_init(mu_Context *ctx)
{
    ctx->text_width = _ui_textWidthCb;
    ctx->text_height = _ui_textHeightCb;
}

/*
 * Clears the framebuffer and draws the commands of the last frame.
 * Expects: a context after mu_end(), a framebuffer and a 0xRRGGBB
 *          background color.
 * Returns: nothing.
 */
void ui_render(mu_Context *ctx, RasFb *fb, TkU32 background)
{
    RasRect full;
    RasRect clip;
    mu_Command *cmd = NULL;
    const UiImageCommand *img = NULL;

    full = ras_rect(
        0,
        0,
        fb->width,
        fb->height
    );
    clip = full;
    ras_fillRect(
        fb,
        full,
        full,
        background | 0xFF000000UL
    );

    while (mu_next_command(ctx, &cmd)) {
        switch (cmd->type) {
        case MU_COMMAND_CLIP:
            clip = ras_intersect(_ui_rect(cmd->clip.rect), full);
            break;
        case MU_COMMAND_RECT:
            ras_fillRect(
                fb,
                clip,
                _ui_rect(cmd->rect.rect),
                _ui_argb(cmd->rect.color)
            );
            break;
        case MU_COMMAND_TEXT:
            _ui_drawText(
                fb,
                clip,
                cmd->text.str,
                cmd->text.pos.x,
                cmd->text.pos.y,
                _ui_argb(cmd->text.color)
            );
            break;
        case MU_COMMAND_ICON:
            _ui_drawIcon(fb, clip, &cmd->icon);
            break;
        case UI_COMMAND_IMAGE:
            img = (const UiImageCommand *)cmd;
            ras_blitImage(
                fb,
                clip,
                &img->image,
                img->rect.x,
                img->rect.y
            );
            break;
        }
    }
}

/*
 * Image control: takes the next layout cell and draws the image there.
 * Expects: a context inside a window; the cell should measure
 *          image size * zoom; image valid until ui_render() runs.
 * Returns: nothing.
 */
void ui_image(mu_Context *ctx, const RasImage *image)
{
    int clipped;
    mu_Rect rect;
    UiImageCommand *cmd = NULL;

    rect = mu_layout_next(ctx);
    clipped = mu_check_clip(ctx, rect);
    if (clipped == MU_CLIP_ALL)
        return;
    /* Same protocol as microui's own draw functions: set the clip only
     * when the control is partly hidden, then reset it. */
    if (clipped == MU_CLIP_PART)
        mu_set_clip(ctx, mu_get_clip_rect(ctx));
    cmd = (UiImageCommand *)mu_push_command(
        ctx,
        UI_COMMAND_IMAGE,
        sizeof(UiImageCommand)
    );
    cmd->rect = rect;
    cmd->image = *image;
    if (clipped) {
        rect.x = 0;
        rect.y = 0;
        rect.w = UI_UNCLIPPED;
        rect.h = UI_UNCLIPPED;
        mu_set_clip(ctx, rect);
    }
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * microui callback: width of a text.
 * Expects: microui's font handle (unused), a string and its length.
 * Returns: the width in pixels.
 */
static int _ui_textWidthCb(mu_Font font, const char *str, int len)
{
    (void)font;
    return _ui_textWidth(str, len);
}

/*
 * microui callback: height of a text line.
 * Expects: microui's font handle (unused).
 * Returns: the line height in pixels.
 */
static int _ui_textHeightCb(mu_Font font)
{
    (void)font;
    return UI_FONT_HEIGHT;
}

/*
 * Draws a microui icon centered in its rectangle.
 * Expects: a framebuffer, the clip and an icon command.
 * Returns: nothing.
 */
static void _ui_drawIcon(RasFb *fb, RasRect clip, const mu_IconCommand *cmd)
{
    int x;
    int y;
    RasMask mask;

    if (!_ui_icon(cmd->id, &mask))
        return;
    x = cmd->rect.x + (cmd->rect.w - mask.src.w) / 2;
    y = cmd->rect.y + (cmd->rect.h - mask.src.h) / 2;
    ras_blitMask(
        fb,
        clip,
        &mask,
        x,
        y,
        _ui_argb(cmd->color)
    );
}

/*
 * Converts a microui color.
 * Expects: a microui color.
 * Returns: the color as 0xAARRGGBB.
 */
static TkU32 _ui_argb(mu_Color c)
{
    return ((TkU32)c.a << 24)
        | ((TkU32)c.r << 16)
        | ((TkU32)c.g << 8)
        | (TkU32)c.b;
}

/*
 * Converts a microui rectangle.
 * Expects: a microui rectangle.
 * Returns: the same rectangle as RasRect.
 */
static RasRect _ui_rect(mu_Rect r)
{
    RasRect out;

    out.x = r.x;
    out.y = r.y;
    out.w = r.w;
    out.h = r.h;
    return out;
}

#ifdef STANDALONE
/* ================================================================
 * 4. Tests: built with -DSTANDALONE as a test program
 * ================================================================ */

#define UI_TEST_BG      0x102030UL

static int _failures = 0;
static int _checks = 0;
static mu_Context _ui_testCtx;

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

static void _ui_testFrame(const RasImage *view, int imageY);

/*
 * Runs the renderer tests: empty frame, window and image control.
 * Expects: nothing.
 * Returns: 0 when every check passes, 1 otherwise.
 */
int main(void)
{
    TkU32 lut[IMG_PALETTE_MAX];
    RasFb fb;
    Img img;
    RasImage view;

    memset(&fb, 0, sizeof(fb));
    ras_resize(&fb, 64, 64);
    mu_init(&_ui_testCtx);
    ui_init(&_ui_testCtx);

    /* empty frame: only the background */
    mu_begin(&_ui_testCtx);
    mu_end(&_ui_testCtx);
    ui_render(&_ui_testCtx, &fb, UI_TEST_BG);
    UI_CHECK(fb.pixels[0] == (0xFF000000UL | UI_TEST_BG));
    UI_CHECK(fb.pixels[64 * 64 - 1] == (0xFF000000UL | UI_TEST_BG));

    /* a red 1x1 image at zoom 4 inside a window */
    img_create(
        &img,
        1,
        1,
        8
    );
    img.paletteCount = 1;
    img.palette[0] = 0xFF0000UL;
    ras_buildLut(&img, lut);
    view.img = &img;
    view.lut = lut;
    view.zoom = 4;

    _ui_testFrame(&view, 0);
    ui_render(&_ui_testCtx, &fb, UI_TEST_BG);
    UI_CHECK(fb.pixels[0] != (0xFF000000UL | UI_TEST_BG));   /* window */
    UI_CHECK(fb.pixels[6 * 64 + 6] == 0xFFFF0000UL);           /* image */

    /* an image scrolled out of view is not drawn */
    _ui_testFrame(&view, 200);
    ui_render(&_ui_testCtx, &fb, UI_TEST_BG);
    UI_CHECK(fb.pixels[6 * 64 + 6] != 0xFFFF0000UL);

    img_free(&img);
    ras_free(&fb);
    printf("ui_rend: %d checks, %d failures\n", _checks, _failures);
    return _failures ? 1 : 0;
}

/*
 * Lays out one frame: a 64x64 window with the image control at imageY.
 * Expects: an image view; imageY >= 0 (empty rows above the image).
 * Returns: nothing.
 */
static void _ui_testFrame(const RasImage *view, int imageY)
{
    int opt;
    int cell[1];
    mu_Rect rect;

    opt = MU_OPT_NOTITLE | MU_OPT_NORESIZE | MU_OPT_NOSCROLL;
    rect = mu_rect(
        0,
        0,
        64,
        64
    );
    mu_begin(&_ui_testCtx);
    if (mu_begin_window_ex(
        &_ui_testCtx,
        "test",
        rect,
        opt
    )) {
        if (imageY > 0) {
            cell[0] = 1;
            mu_layout_row(
                &_ui_testCtx,
                1,
                cell,
                imageY
            );
            mu_layout_next(&_ui_testCtx);
        }
        cell[0] = view->img->width * view->zoom;
        mu_layout_row(
            &_ui_testCtx,
            1,
            cell,
            view->img->height * view->zoom
        );
        ui_image(&_ui_testCtx, view);
        mu_end_window(&_ui_testCtx);
    }
    mu_end(&_ui_testCtx);
}

#endif /* STANDALONE */
