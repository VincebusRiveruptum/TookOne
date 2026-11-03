/*
 * raster/private.h - internals of the raster module.
 * Included only by the module's own .c files.
 */
#ifndef CORE_RASTER_PRIVATE_H
#define CORE_RASTER_PRIVATE_H

#include "raster.h"

/* Visible part of rect: rect, clip and the framebuffer intersected. */
RasRect _ras_visible(const RasFb *fb, RasRect clip, RasRect rect);

/* src blended over dst with alpha a (0..255); result is opaque. */
TkU32 _ras_blend(TkU32 dst, TkU32 src, TkU32 a);

#endif /* CORE_RASTER_PRIVATE_H */
