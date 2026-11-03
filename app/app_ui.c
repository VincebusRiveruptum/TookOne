/*
 * app_ui - layout of the main window (module: app).
 */
#include "private.h"

/* ---- Prototypes of file-private functions ---- */
static void _app_frame(App *app);
static void _app_toolbar(App *app);
static void _app_canvas(App *app);

/* ================================================================
 * 2. Module internal (declared in private.h)
 * ================================================================ */

/*
 * Lays out one UI frame and renders it into the framebuffer.
 * Expects: an initialized App.
 * Returns: nothing.
 */
void _app_draw(App *app)
{
    _app_frame(app);
    ui_render(app->ctx, &app->fb, APP_BACKGROUND);
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * Declares the main window (toolbar + canvas) for this frame.
 * Expects: an initialized App.
 * Returns: nothing.
 */
static void _app_frame(App *app)
{
    int opt;
    mu_Container *win = NULL;

    opt = MU_OPT_NOTITLE | MU_OPT_NORESIZE | MU_OPT_NOCLOSE;
    mu_begin(app->ctx);
    win = mu_get_container(app->ctx, APP_TITLE);
    win->rect = mu_rect(
        0,
        0,
        app->fb.width,
        app->fb.height
    );
    if (mu_begin_window_ex(
        app->ctx,
        APP_TITLE,
        win->rect,
        opt
    )) {
        _app_toolbar(app);
        _app_canvas(app);
        mu_end_window(app->ctx);
    }
    mu_end(app->ctx);
}

/*
 * Declares the toolbar: zoom buttons, zoom factor and status text.
 * Expects: an App inside the main window.
 * Returns: nothing.
 */
static void _app_toolbar(App *app)
{
    static const int widths[5] = { 70, 70, 50, 60, -1 };
    char zoom[16];

    mu_layout_row(
        app->ctx,
        5,
        widths,
        0
    );
    if (mu_button(app->ctx, "Zoom -") && app->hasImage)
        _app_setZoomStep(app, app->zoomStep - 1);
    if (mu_button(app->ctx, "Zoom +") && app->hasImage)
        _app_setZoomStep(app, app->zoomStep + 1);
    if (mu_button(app->ctx, "1:1") && app->hasImage)
        _app_setZoomStep(app, 0);
    sprintf(zoom, "x%d", _app_zoom(app));
    mu_label(app->ctx, zoom);
    mu_label(app->ctx, app->status);
}

/*
 * Declares the scrollable canvas with the image, or a hint if empty.
 * Expects: an App inside the main window.
 * Returns: nothing.
 */
static void _app_canvas(App *app)
{
    static const int fill[1] = { -1 };
    int cell[1];

    mu_layout_row(
        app->ctx,
        1,
        fill,
        -1
    );
    mu_begin_panel(app->ctx, "canvas");
    if (app->hasImage) {
        cell[0] = app->img.width * _app_zoom(app);
        mu_layout_row(
            app->ctx,
            1,
            cell,
            app->img.height * _app_zoom(app)
        );
        ui_image(app->ctx, &app->view);
    } else {
        mu_layout_row(
            app->ctx,
            1,
            fill,
            0
        );
        mu_label(app->ctx, "Drop a BMP file here, or run: tookone file.bmp");
    }
    mu_end_panel(app->ctx);
}
