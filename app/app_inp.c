/*
 * app_inp - routes platform events to microui and to the app (module: app).
 */
#include "private.h"

/* ---- Prototypes of file-private functions ---- */
static void _app_onKeyDown(App *app, const HalEvent *ev);
static void _app_onWheel(App *app, const HalEvent *ev);
static int _app_muKey(int key);
static int _app_muButton(int button);

/* ================================================================
 * 2. Module internal (declared in private.h)
 * ================================================================ */

/*
 * Handles one platform event.
 * Expects: an initialized App and an event from hal_waitEvent.
 * Returns: nothing.
 */
void _app_handleEvent(App *app, const HalEvent *ev)
{
    switch (ev->type) {
    case HAL_EV_QUIT:
        app->running = 0;
        break;
    case HAL_EV_RESIZE:
        ras_resize(&app->fb, ev->width, ev->height);
        break;
    case HAL_EV_MOUSEMOVE:
        mu_input_mousemove(app->ctx, ev->x, ev->y);
        break;
    case HAL_EV_MOUSEDOWN:
        mu_input_mousedown(
            app->ctx,
            ev->x,
            ev->y,
            _app_muButton(ev->button)
        );
        break;
    case HAL_EV_MOUSEUP:
        mu_input_mouseup(
            app->ctx,
            ev->x,
            ev->y,
            _app_muButton(ev->button)
        );
        break;
    case HAL_EV_WHEEL:
        _app_onWheel(app, ev);
        break;
    case HAL_EV_KEYDOWN:
        _app_onKeyDown(app, ev);
        break;
    case HAL_EV_KEYUP:
        if (_app_muKey(ev->key))
            mu_input_keyup(app->ctx, _app_muKey(ev->key));
        break;
    case HAL_EV_TEXT:
        mu_input_text(app->ctx, ev->text);
        break;
    case HAL_EV_DROPFILE:
        _app_loadFile(app, ev->path);
        break;
    }
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * Handles a key press: microui keys and zoom shortcuts (+, -, 0).
 * Expects: an App and a HAL_EV_KEYDOWN event.
 * Returns: nothing.
 */
static void _app_onKeyDown(App *app, const HalEvent *ev)
{
    if (_app_muKey(ev->key))
        mu_input_keydown(app->ctx, _app_muKey(ev->key));
    if (!app->hasImage)
        return;
    if (ev->key == HAL_KEY_PLUS)
        _app_setZoomStep(app, app->zoomStep + 1);
    else if (ev->key == HAL_KEY_MINUS)
        _app_setZoomStep(app, app->zoomStep - 1);
    else if (ev->key == HAL_KEY_0)
        _app_setZoomStep(app, 0);
}

/*
 * Handles the mouse wheel: Ctrl zooms, otherwise it scrolls.
 * Expects: an App and a HAL_EV_WHEEL event.
 * Returns: nothing.
 */
static void _app_onWheel(App *app, const HalEvent *ev)
{
    if ((ev->mods & HAL_MOD_CTRL) && app->hasImage) {
        _app_setZoomStep(app, app->zoomStep + ev->wheel);
        return;
    }
    mu_input_scroll(app->ctx, 0, -ev->wheel * APP_SCROLL_STEP);
}

/*
 * Maps a HAL key to a microui key.
 * Expects: a HAL_KEY_* value.
 * Returns: the MU_KEY_* flag, or 0 if microui does not use it.
 */
static int _app_muKey(int key)
{
    switch (key) {
    case HAL_KEY_SHIFT:     return MU_KEY_SHIFT;
    case HAL_KEY_CTRL:      return MU_KEY_CTRL;
    case HAL_KEY_ALT:       return MU_KEY_ALT;
    case HAL_KEY_BACKSPACE: return MU_KEY_BACKSPACE;
    case HAL_KEY_RETURN:    return MU_KEY_RETURN;
    }
    return 0;
}

/*
 * Maps a HAL mouse button to a microui button.
 * Expects: a HAL_MOUSE_* value.
 * Returns: the MU_MOUSE_* flag.
 */
static int _app_muButton(int button)
{
    if (button == HAL_MOUSE_LEFT)
        return MU_MOUSE_LEFT;
    if (button == HAL_MOUSE_RIGHT)
        return MU_MOUSE_RIGHT;
    return MU_MOUSE_MIDDLE;
}
