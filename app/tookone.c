/*
 * tookone - application entry point and main loop.
 *
 * Slice 1: open a BMP (command line or drag & drop) and view it with zoom.
 *
 *   tookone [file.bmp] [--shot out.ppm]
 *
 * --shot renders one frame into a PPM file and exits (UI tests).
 */
#include <stdio.h>
#include <string.h>
#include "../platform/plat.h"
#include "../external/microui/microui.h"
#include "../ui/uirend.h"
#include "../core/bmp.h"

#define APP_TITLE           "TookOne"
#define APP_WIDTH           960
#define APP_HEIGHT          640
#define APP_STATUS_LEN      255
#define APP_NAME_LEN        120
#define APP_BACKGROUND      0x202020UL
#define APP_SCROLL_STEP     30
#define APP_MARGIN_X        40
#define APP_MARGIN_Y        90

static const int _zooms[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24, 32 };

#define APP_ZOOM_COUNT  ((int)(sizeof(_zooms) / sizeof(_zooms[0])))

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
} App;

static mu_Context _ctx;

/* ---- helpers ---- */

/* File name without directories, truncated to APP_NAME_LEN chars. */
static void _baseName(const char *path, char *out)
{
    const char *p = NULL;
    const char *name = NULL;

    name = path;
    for (p = path; *p; p++) {
        if (*p == '/' || *p == '\\')
            name = p + 1;
    }
    strncpy(out, name, APP_NAME_LEN);
    out[APP_NAME_LEN] = '\0';
}

static int _zoom(const App *app)
{
    return _zooms[app->zoomStep];
}

static void _setZoomStep(App *app, int step)
{
    if (step < 0)
        step = 0;
    if (step >= APP_ZOOM_COUNT)
        step = APP_ZOOM_COUNT - 1;
    app->zoomStep = step;
    app->view.zoom = _zoom(app);
}

/* Largest zoom that fits the image in the window (at least 1:1). */
static void _fitZoom(App *app)
{
    int step;
    int fitW;
    int fitH;

    fitW = app->fb.width - APP_MARGIN_X;
    fitH = app->fb.height - APP_MARGIN_Y;
    for (step = APP_ZOOM_COUNT - 1; step > 0; step--) {
        if (app->img.width * _zooms[step] <= fitW
            && app->img.height * _zooms[step] <= fitH)
            break;
    }
    _setZoomStep(app, step);
}

static void _loadFile(App *app, const char *path)
{
    int err;
    char name[APP_NAME_LEN + 1];
    Img img;

    _baseName(path, name);
    err = bmp_load(path, &img);
    if (err != BMP_OK) {
        sprintf(
            app->status,
            "%s: %s",
            name,
            bmp_strError(err)
        );
        return;
    }

    img_free(&app->img);
    app->img = img;
    app->hasImage = 1;
    ras_buildLut(&app->img, app->lut);
    app->view.img = &app->img;
    app->view.lut = app->lut;
    _fitZoom(app);

    sprintf(
        app->status,
        "%s  %dx%d  %d bpp",
        name,
        img.width,
        img.height,
        img.bpp
    );
    sprintf(
        app->title,
        "%s - %s",
        APP_TITLE,
        name
    );
    plat_setTitle(app->title);
}

/* Writes the framebuffer as a binary PPM (P6). Returns 1 on success. */
static int _writePpm(const RasFb *fb, const char *path)
{
    int i;
    int count;
    unsigned char rgb[3];
    TkU32 px;
    FILE *fp = NULL;

    fp = fopen(path, "wb");
    if (!fp)
        return 0;
    fprintf(
        fp,
        "P6\n%d %d\n255\n",
        fb->width,
        fb->height
    );
    count = fb->width * fb->height;
    for (i = 0; i < count; i++) {
        px = fb->pixels[i];
        rgb[0] = (unsigned char)((px >> 16) & 0xFF);
        rgb[1] = (unsigned char)((px >> 8) & 0xFF);
        rgb[2] = (unsigned char)(px & 0xFF);
        fwrite(
            rgb,
            1,
            3,
            fp
        );
    }
    return fclose(fp) == 0;
}

/* ---- input ---- */

static int _muKey(int key)
{
    switch (key) {
    case PLAT_KEY_SHIFT:     return MU_KEY_SHIFT;
    case PLAT_KEY_CTRL:      return MU_KEY_CTRL;
    case PLAT_KEY_ALT:       return MU_KEY_ALT;
    case PLAT_KEY_BACKSPACE: return MU_KEY_BACKSPACE;
    case PLAT_KEY_RETURN:    return MU_KEY_RETURN;
    }
    return 0;
}

static int _muButton(int button)
{
    if (button == PLAT_MOUSE_LEFT)
        return MU_MOUSE_LEFT;
    if (button == PLAT_MOUSE_RIGHT)
        return MU_MOUSE_RIGHT;
    return MU_MOUSE_MIDDLE;
}

static void _onKeyDown(App *app, const PlatEvent *ev)
{
    if (_muKey(ev->key))
        mu_input_keydown(&_ctx, _muKey(ev->key));
    if (!app->hasImage)
        return;
    if (ev->key == PLAT_KEY_PLUS)
        _setZoomStep(app, app->zoomStep + 1);
    else if (ev->key == PLAT_KEY_MINUS)
        _setZoomStep(app, app->zoomStep - 1);
    else if (ev->key == PLAT_KEY_0)
        _setZoomStep(app, 0);
}

static void _onWheel(App *app, const PlatEvent *ev)
{
    if ((ev->mods & PLAT_MOD_CTRL) && app->hasImage) {
        _setZoomStep(app, app->zoomStep + ev->wheel);
        return;
    }
    mu_input_scroll(&_ctx, 0, -ev->wheel * APP_SCROLL_STEP);
}

static void _handleEvent(App *app, const PlatEvent *ev)
{
    switch (ev->type) {
    case PLAT_EV_QUIT:
        app->running = 0;
        break;
    case PLAT_EV_RESIZE:
        ras_resize(&app->fb, ev->width, ev->height);
        break;
    case PLAT_EV_MOUSEMOVE:
        mu_input_mousemove(&_ctx, ev->x, ev->y);
        break;
    case PLAT_EV_MOUSEDOWN:
        mu_input_mousedown(
            &_ctx,
            ev->x,
            ev->y,
            _muButton(ev->button)
        );
        break;
    case PLAT_EV_MOUSEUP:
        mu_input_mouseup(
            &_ctx,
            ev->x,
            ev->y,
            _muButton(ev->button)
        );
        break;
    case PLAT_EV_WHEEL:
        _onWheel(app, ev);
        break;
    case PLAT_EV_KEYDOWN:
        _onKeyDown(app, ev);
        break;
    case PLAT_EV_KEYUP:
        if (_muKey(ev->key))
            mu_input_keyup(&_ctx, _muKey(ev->key));
        break;
    case PLAT_EV_TEXT:
        mu_input_text(&_ctx, ev->text);
        break;
    case PLAT_EV_DROPFILE:
        _loadFile(app, ev->path);
        break;
    }
}

/* ---- UI ---- */

static void _toolbar(App *app)
{
    static const int widths[5] = { 70, 70, 50, 60, -1 };
    char zoom[16];

    mu_layout_row(
        &_ctx,
        5,
        widths,
        0
    );
    if (mu_button(&_ctx, "Zoom -") && app->hasImage)
        _setZoomStep(app, app->zoomStep - 1);
    if (mu_button(&_ctx, "Zoom +") && app->hasImage)
        _setZoomStep(app, app->zoomStep + 1);
    if (mu_button(&_ctx, "1:1") && app->hasImage)
        _setZoomStep(app, 0);
    sprintf(zoom, "x%d", _zoom(app));
    mu_label(&_ctx, zoom);
    mu_label(&_ctx, app->status);
}

static void _canvas(App *app)
{
    static const int fill[1] = { -1 };
    int cell[1];

    mu_layout_row(
        &_ctx,
        1,
        fill,
        -1
    );
    mu_begin_panel(&_ctx, "canvas");
    if (app->hasImage) {
        cell[0] = app->img.width * _zoom(app);
        mu_layout_row(
            &_ctx,
            1,
            cell,
            app->img.height * _zoom(app)
        );
        uir_image(&_ctx, &app->view);
    } else {
        mu_layout_row(
            &_ctx,
            1,
            fill,
            0
        );
        mu_label(&_ctx, "Drop a BMP file here, or run: tookone file.bmp");
    }
    mu_end_panel(&_ctx);
}

static void _frame(App *app)
{
    int opt;
    mu_Container *win = NULL;

    opt = MU_OPT_NOTITLE | MU_OPT_NORESIZE | MU_OPT_NOCLOSE;
    mu_begin(&_ctx);
    win = mu_get_container(&_ctx, APP_TITLE);
    win->rect = mu_rect(
        0,
        0,
        app->fb.width,
        app->fb.height
    );
    if (mu_begin_window_ex(
        &_ctx,
        APP_TITLE,
        win->rect,
        opt
    )) {
        _toolbar(app);
        _canvas(app);
        mu_end_window(&_ctx);
    }
    mu_end(&_ctx);
}

static void _draw(App *app)
{
    _frame(app);
    uir_render(&_ctx, &app->fb, APP_BACKGROUND);
}

/* ---- main ---- */

static int _init(App *app)
{
    int width;
    int height;

    memset(app, 0, sizeof(*app));
    app->running = 1;
    app->view.zoom = 1;
    strcpy(app->status, "No file");

    if (plat_init(APP_TITLE, APP_WIDTH, APP_HEIGHT) != PLAT_OK) {
        fprintf(stderr, "tookone: cannot open window\n");
        return 0;
    }
    plat_windowSize(&width, &height);
    if (ras_resize(&app->fb, width, height) != RAS_OK) {
        fprintf(stderr, "tookone: out of memory\n");
        plat_shutdown();
        return 0;
    }
    mu_init(&_ctx);
    uir_init(&_ctx);
    return 1;
}

static void _shutdown(App *app)
{
    img_free(&app->img);
    ras_free(&app->fb);
    plat_shutdown();
}

int main(int argc, char **argv)
{
    int i;
    int frames;
    App app;
    PlatEvent ev;
    const char *file = NULL;
    const char *shot = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--shot") == 0 && i + 1 < argc)
            shot = argv[++i];
        else
            file = argv[i];
    }

    if (!_init(&app))
        return 1;
    if (file)
        _loadFile(&app, file);

    if (shot) {
        /* Two frames: microui settles layout and hover on the second. */
        _draw(&app);
        _draw(&app);
        i = _writePpm(&app.fb, shot);
        _shutdown(&app);
        return i ? 0 : 1;
    }

    frames = 2;
    while (app.running) {
        if (frames == 0 && plat_waitEvent(&ev, -1)) {
            _handleEvent(&app, &ev);
            frames = 2;
        }
        while (plat_waitEvent(&ev, 0)) {
            _handleEvent(&app, &ev);
            frames = 2;
        }
        if (frames > 0) {
            _draw(&app);
            plat_present(app.fb.pixels, app.fb.width, app.fb.height);
            frames--;
        }
    }

    _shutdown(&app);
    return 0;
}
