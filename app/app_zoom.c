/*
 * app_zoom - zoom levels of the canvas (module: app).
 */
#include "private.h"

static const int _app_zooms[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24, 32 };

#define APP_ZOOM_COUNT  ((int)(sizeof(_app_zooms) / sizeof(_app_zooms[0])))

/* ================================================================
 * 2. Module internal (declared in private.h)
 * ================================================================ */

/*
 * Gets the current zoom factor.
 * Expects: an App.
 * Returns: the factor (1, 2, 3, 4, 6 ... 32).
 */
int _app_zoom(const App *app)
{
    return _app_zooms[app->zoomStep];
}

/*
 * Selects a zoom level, clamped to the available ones.
 * Expects: an App and a step (0 = 1:1, may be out of range).
 * Returns: nothing.
 */
void _app_setZoomStep(App *app, int step)
{
    if (step < 0)
        step = 0;
    if (step >= APP_ZOOM_COUNT)
        step = APP_ZOOM_COUNT - 1;
    app->zoomStep = step;
    app->view.zoom = _app_zoom(app);
}

/*
 * Selects the largest zoom that fits the image in the window (min 1:1).
 * Expects: an App with an image and a framebuffer.
 * Returns: nothing.
 */
void _app_fitZoom(App *app)
{
    int step;
    int fitW;
    int fitH;

    fitW = app->fb.width - APP_MARGIN_X;
    fitH = app->fb.height - APP_MARGIN_Y;
    for (step = APP_ZOOM_COUNT - 1; step > 0; step--) {
        if (app->img.width * _app_zooms[step] <= fitW
            && app->img.height * _app_zooms[step] <= fitH)
            break;
    }
    _app_setZoomStep(app, step);
}
