#include <string.h>
#include "uirend.h"
#include "font.h"

/* Custom command types start after microui's own. */
#define UIR_COMMAND_IMAGE   (MU_COMMAND_MAX + 1)

/* microui's "no clipping" rectangle size (unclipped_rect in microui.c) */
#define UIR_UNCLIPPED       0x1000000

typedef struct UirImageCommand {
    mu_BaseCommand base;
    mu_Rect rect;
    RasImage image;
} UirImageCommand;

static int _textWidth(mu_Font font, const char *str, int len)
{
    (void)font;
    return font_textWidth(str, len);
}

static int _textHeight(mu_Font font)
{
    (void)font;
    return FONT_HEIGHT;
}

void uir_init(mu_Context *ctx)
{
    ctx->text_width = _textWidth;
    ctx->text_height = _textHeight;
}

static TkU32 _argb(mu_Color c)
{
    return ((TkU32)c.a << 24)
        | ((TkU32)c.r << 16)
        | ((TkU32)c.g << 8)
        | (TkU32)c.b;
}

static RasRect _rect(mu_Rect r)
{
    return ras_rect(
        r.x,
        r.y,
        r.w,
        r.h
    );
}

static void _drawIcon(RasFb *fb, RasRect clip, const mu_IconCommand *cmd)
{
    int x;
    int y;
    RasMask mask;

    if (!font_icon(cmd->id, &mask))
        return;
    x = cmd->rect.x + (cmd->rect.w - mask.src.w) / 2;
    y = cmd->rect.y + (cmd->rect.h - mask.src.h) / 2;
    ras_blitMask(
        fb,
        clip,
        &mask,
        x,
        y,
        _argb(cmd->color)
    );
}

void uir_render(mu_Context *ctx, RasFb *fb, TkU32 background)
{
    RasRect full;
    RasRect clip;
    mu_Command *cmd = NULL;
    const UirImageCommand *img = NULL;

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
            clip = ras_intersect(_rect(cmd->clip.rect), full);
            break;
        case MU_COMMAND_RECT:
            ras_fillRect(
                fb,
                clip,
                _rect(cmd->rect.rect),
                _argb(cmd->rect.color)
            );
            break;
        case MU_COMMAND_TEXT:
            font_drawText(
                fb,
                clip,
                cmd->text.str,
                -1,
                cmd->text.pos.x,
                cmd->text.pos.y,
                _argb(cmd->text.color)
            );
            break;
        case MU_COMMAND_ICON:
            _drawIcon(fb, clip, &cmd->icon);
            break;
        case UIR_COMMAND_IMAGE:
            img = (const UirImageCommand *)cmd;
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

void uir_image(mu_Context *ctx, const RasImage *image)
{
    int clipped;
    mu_Rect rect;
    UirImageCommand *cmd = NULL;

    rect = mu_layout_next(ctx);
    clipped = mu_check_clip(ctx, rect);
    if (clipped == MU_CLIP_ALL)
        return;
    /* Same protocol as microui's own draw functions: set the clip only
     * when the control is partly hidden, then reset it. */
    if (clipped == MU_CLIP_PART)
        mu_set_clip(ctx, mu_get_clip_rect(ctx));
    cmd = (UirImageCommand *)mu_push_command(
        ctx,
        UIR_COMMAND_IMAGE,
        sizeof(UirImageCommand)
    );
    cmd->rect = rect;
    cmd->image = *image;
    if (clipped)
        mu_set_clip(ctx, mu_rect(
            0,
            0,
            UIR_UNCLIPPED,
            UIR_UNCLIPPED
        ));
}
