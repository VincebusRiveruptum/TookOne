/*
 * main - entry point and main loop (module: app).
 *
 *   tookone [file.bmp] [--shot out.ppm]
 *
 * --shot renders one frame into a PPM file and exits (UI tests without a
 * screen).
 */
#include "private.h"

static mu_Context _app_mu;

/* ---- Prototypes of file-private functions ---- */
static int _app_init(App *app);
static void _app_shutdown(App *app);
static void _app_run(App *app);
static int _app_shot(App *app, const char *path);

/* ================================================================
 * 1. Entry point
 * ================================================================ */

/*
 * Starts TookOne.
 * Expects: optional BMP path and optional "--shot out.ppm".
 * Returns: 0 on success, 1 if the window or the screenshot fails.
 */
int main(int argc, char **argv)
{
    int i;
    int ok;
    App app;
    const char *file = NULL;
    const char *shot = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--shot") == 0 && i + 1 < argc)
            shot = argv[++i];
        else
            file = argv[i];
    }

    if (!_app_init(&app))
        return 1;
    if (file)
        _app_loadFile(&app, file);

    ok = 1;
    if (shot)
        ok = _app_shot(&app, shot);
    else
        _app_run(&app);

    _app_shutdown(&app);
    return ok ? 0 : 1;
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * Opens the window and prepares the UI and the framebuffer.
 * Expects: an App to initialize.
 * Returns: 1 on success, 0 on failure (message on stderr).
 */
static int _app_init(App *app)
{
    int width;
    int height;

    memset(app, 0, sizeof(*app));
    app->running = 1;
    app->view.zoom = 1;
    app->ctx = &_app_mu;
    strcpy(app->status, "No file");

    if (hal_init(APP_TITLE, APP_WIDTH, APP_HEIGHT) != HAL_OK) {
        fprintf(stderr, "tookone: cannot open window\n");
        return 0;
    }
    hal_windowSize(&width, &height);
    if (ras_resize(&app->fb, width, height) != RAS_OK) {
        fprintf(stderr, "tookone: out of memory\n");
        hal_shutdown();
        return 0;
    }
    mu_init(app->ctx);
    ui_init(app->ctx);
    return 1;
}

/*
 * Releases everything opened by _app_init.
 * Expects: an initialized App.
 * Returns: nothing.
 */
static void _app_shutdown(App *app)
{
    img_free(&app->img);
    ras_free(&app->fb);
    hal_shutdown();
}

/*
 * Main loop: waits for events and redraws only after them.
 * Expects: an initialized App.
 * Returns: when the user closes the window.
 */
static void _app_run(App *app)
{
    int frames;
    HalEvent ev;

    /* microui settles hover / focus on the frame after an input */
    frames = 2;
    while (app->running) {
        if (frames == 0 && hal_waitEvent(&ev, -1)) {
            _app_handleEvent(app, &ev);
            frames = 2;
        }
        while (hal_waitEvent(&ev, 0)) {
            _app_handleEvent(app, &ev);
            frames = 2;
        }
        if (frames > 0) {
            _app_draw(app);
            hal_present(app->fb.pixels, app->fb.width, app->fb.height);
            frames--;
        }
    }
}

/*
 * Renders two frames and saves the framebuffer as a PPM file.
 * Expects: an initialized App and the output path.
 * Returns: 1 on success, 0 if the file cannot be written.
 */
static int _app_shot(App *app, const char *path)
{
    _app_draw(app);
    _app_draw(app);
    return _app_writePpm(&app->fb, path);
}
