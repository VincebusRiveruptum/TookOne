/*
 * app/private.h - internals of the app module (level 5).
 * Included only by the module's own .c files; app has no public header.
 */
#ifndef APP_PRIVATE_H
#define APP_PRIVATE_H

#include "../hal/hal.h"
#include "../ui/ui.h"

#define APP_TITLE           "TookOne"
#define APP_WIDTH           960
#define APP_HEIGHT          640
#define APP_STATUS_LEN      255
#define APP_NAME_LEN        120
#define APP_BACKGROUND      0x202020UL
#define APP_SCROLL_STEP     30
#define APP_MARGIN_X        40
#define APP_MARGIN_Y        90

/* Application state, passed by pointer to every app function. */
typedef struct App {
    int running;
    int zoomStep;
    int hasImage;
    char status[APP_STATUS_LEN + 1];
    char title[APP_STATUS_LEN + 1];
    TkU32 lut[IMG_PALETTE_MAX];
    Img img;
    RasFb fb;
    RasImage view;
    mu_Context *ctx;
} App;

/* ---- Zoom (app_zoom.c) ---- */

int _app_zoom(const App *app);
void _app_setZoomStep(App *app, int step);
void _app_fitZoom(App *app);

/* ---- Files (app_file.c) ---- */

void _app_loadFile(App *app, const char *path);
int _app_writePpm(const RasFb *fb, const char *path);

/* ---- Input (app_inp.c) ---- */

void _app_handleEvent(App *app, const HalEvent *ev);

/* ---- Layout and drawing (app_ui.c) ---- */

void _app_draw(App *app);

#endif /* APP_PRIVATE_H */
