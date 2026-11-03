/*
 * ui - draws microui command lists into a framebuffer and provides the
 * custom controls of TookOne (level 4).
 */
#ifndef UI_UI_H
#define UI_UI_H

#include "../external/microui/microui.h"
#include "../core/raster/raster.h"

/* ---- Renderer (ui_rend.c) ---- */

void ui_init(mu_Context *ctx);
void ui_render(mu_Context *ctx, RasFb *fb, TkU32 background);

/* ---- Controls (ui_rend.c) ---- */

void ui_image(mu_Context *ctx, const RasImage *image);

#endif /* UI_UI_H */
