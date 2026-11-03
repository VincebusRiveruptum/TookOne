/*
 * uirend - draws microui command lists into a RasFb (software renderer)
 * and provides the image control used by the canvas.
 */
#ifndef UIREND_H
#define UIREND_H

#include "../external/microui/microui.h"
#include "../core/raster.h"

/* Sets microui's text measuring callbacks. Call after mu_init(). */
void uir_init(mu_Context *ctx);

/* Clears the framebuffer and draws every command of the last frame. */
void uir_render(mu_Context *ctx, RasFb *fb, TkU32 background);

/*
 * Image control: takes the next layout cell and draws the image there,
 * scaled by image->zoom. The cell should be image size * zoom.
 * image must stay valid until uir_render() runs.
 */
void uir_image(mu_Context *ctx, const RasImage *image);

#endif /* UIREND_H */
