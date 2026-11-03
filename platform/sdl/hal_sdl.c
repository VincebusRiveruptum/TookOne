/*
 * hal_sdl - SDL3 backend of the hal contract (platform: sdl).
 *
 * SDL3 headers are not C89, so this file is built with -std=c99
 * (see c-code-stlye.md, scope). The code itself follows the C89 style.
 */
#include <string.h>
#include <SDL3/SDL.h>
#include "../../hal/hal.h"

static int _hal_texWidth = 0;
static int _hal_texHeight = 0;
static SDL_Window *_hal_window = NULL;
static SDL_Renderer *_hal_renderer = NULL;
static SDL_Texture *_hal_texture = NULL;

/* ---- Prototypes of file-private functions ---- */
static int _hal_translate(const SDL_Event *e, HalEvent *ev);
static int _hal_ensureTexture(int width, int height);
static int _hal_mods(void);
static int _hal_key(SDL_Keycode k);
static int _hal_button(Uint8 b);
static void _hal_copy(char *dst, const char *src, size_t len);

/* ================================================================
 * 1. Public API (declared in hal/hal.h)
 * ================================================================ */

/*
 * Opens the application window.
 * Expects: a title and the initial size in pixels.
 * Returns: HAL_OK or HAL_ERR_INIT.
 */
int hal_init(const char *title, int width, int height)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return HAL_ERR_INIT;

    _hal_window = SDL_CreateWindow(
        title,
        width,
        height,
        SDL_WINDOW_RESIZABLE
    );
    if (!_hal_window) {
        SDL_Quit();
        return HAL_ERR_INIT;
    }
    _hal_renderer = SDL_CreateRenderer(_hal_window, NULL);
    if (!_hal_renderer) {
        SDL_DestroyWindow(_hal_window);
        _hal_window = NULL;
        SDL_Quit();
        return HAL_ERR_INIT;
    }
    SDL_StartTextInput(_hal_window);
    return HAL_OK;
}

/*
 * Closes the window and releases the platform.
 * Expects: nothing (safe after a failed hal_init).
 * Returns: nothing.
 */
void hal_shutdown(void)
{
    if (_hal_texture)
        SDL_DestroyTexture(_hal_texture);
    if (_hal_renderer)
        SDL_DestroyRenderer(_hal_renderer);
    if (_hal_window)
        SDL_DestroyWindow(_hal_window);
    _hal_texture = NULL;
    _hal_renderer = NULL;
    _hal_window = NULL;
    SDL_Quit();
}

/*
 * Gets the current window size.
 * Expects: two ints to fill.
 * Returns: nothing.
 */
void hal_windowSize(int *width, int *height)
{
    SDL_GetWindowSize(_hal_window, width, height);
}

/*
 * Changes the window title.
 * Expects: a UTF-8 title.
 * Returns: nothing.
 */
void hal_setTitle(const char *title)
{
    SDL_SetWindowTitle(_hal_window, title);
}

/*
 * Shows a framebuffer in the window.
 * Expects: width * height pixels in 0xAARRGGBB format.
 * Returns: nothing.
 */
void hal_present(const void *argb, int width, int height)
{
    if (!_hal_ensureTexture(width, height))
        return;
    SDL_UpdateTexture(
        _hal_texture,
        NULL,
        argb,
        width * 4
    );
    SDL_RenderClear(_hal_renderer);
    SDL_RenderTexture(
        _hal_renderer,
        _hal_texture,
        NULL,
        NULL
    );
    SDL_RenderPresent(_hal_renderer);
}

/*
 * Waits for an event the application understands.
 * Expects: an event to fill and a timeout in ms (-1 = forever).
 * Returns: 1 when an event was received, 0 on timeout.
 */
int hal_waitEvent(HalEvent *ev, int timeoutMs)
{
    SDL_Event e;

    for (;;) {
        if (!SDL_WaitEventTimeout(&e, timeoutMs))
            return 0;
        if (_hal_translate(&e, ev))
            return 1;
        /* Ignored event: keep waiting, unless polling. */
        if (timeoutMs == 0)
            return 0;
    }
}

/*
 * Gets the time since hal_init.
 * Expects: nothing.
 * Returns: milliseconds.
 */
unsigned long hal_ticksMs(void)
{
    return (unsigned long)SDL_GetTicks();
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * Translates an SDL event into a HalEvent.
 * Expects: an SDL event and the HalEvent to fill.
 * Returns: 1 if TookOne uses the event, 0 to ignore it.
 */
static int _hal_translate(const SDL_Event *e, HalEvent *ev)
{
    memset(ev, 0, sizeof(*ev));
    ev->mods = _hal_mods();

    switch (e->type) {
    case SDL_EVENT_QUIT:
        ev->type = HAL_EV_QUIT;
        return 1;
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        ev->type = HAL_EV_RESIZE;
        SDL_GetWindowSize(_hal_window, &ev->width, &ev->height);
        return 1;
    case SDL_EVENT_WINDOW_EXPOSED:
        ev->type = HAL_EV_EXPOSE;
        return 1;
    case SDL_EVENT_MOUSE_MOTION:
        ev->type = HAL_EV_MOUSEMOVE;
        ev->x = (int)e->motion.x;
        ev->y = (int)e->motion.y;
        return 1;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        ev->type = e->type == SDL_EVENT_MOUSE_BUTTON_DOWN
            ? HAL_EV_MOUSEDOWN
            : HAL_EV_MOUSEUP;
        ev->x = (int)e->button.x;
        ev->y = (int)e->button.y;
        ev->button = _hal_button(e->button.button);
        return ev->button != 0;
    case SDL_EVENT_MOUSE_WHEEL:
        if (e->wheel.y == 0)
            return 0;
        ev->type = HAL_EV_WHEEL;
        ev->wheel = e->wheel.y > 0 ? 1 : -1;
        return 1;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        ev->type = e->type == SDL_EVENT_KEY_DOWN
            ? HAL_EV_KEYDOWN
            : HAL_EV_KEYUP;
        ev->key = _hal_key(e->key.key);
        return ev->key != HAL_KEY_UNKNOWN;
    case SDL_EVENT_TEXT_INPUT:
        ev->type = HAL_EV_TEXT;
        _hal_copy(ev->text, e->text.text, HAL_TEXT_LEN);
        return 1;
    case SDL_EVENT_DROP_FILE:
        ev->type = HAL_EV_DROPFILE;
        _hal_copy(ev->path, e->drop.data, HAL_PATH_LEN);
        return 1;
    }
    return 0;
}

/*
 * Makes sure the streaming texture matches the framebuffer size.
 * Expects: the framebuffer size.
 * Returns: 1 when the texture is ready, 0 if it cannot be created.
 */
static int _hal_ensureTexture(int width, int height)
{
    if (_hal_texture && _hal_texWidth == width && _hal_texHeight == height)
        return 1;
    if (_hal_texture)
        SDL_DestroyTexture(_hal_texture);
    _hal_texture = SDL_CreateTexture(
        _hal_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );
    if (!_hal_texture)
        return 0;
    SDL_SetTextureScaleMode(_hal_texture, SDL_SCALEMODE_NEAREST);
    _hal_texWidth = width;
    _hal_texHeight = height;
    return 1;
}

/*
 * Reads the keyboard modifiers.
 * Expects: nothing.
 * Returns: HAL_MOD_* flags.
 */
static int _hal_mods(void)
{
    int mods = 0;
    SDL_Keymod m;

    m = SDL_GetModState();
    if (m & SDL_KMOD_SHIFT)
        mods |= HAL_MOD_SHIFT;
    if (m & SDL_KMOD_CTRL)
        mods |= HAL_MOD_CTRL;
    if (m & SDL_KMOD_ALT)
        mods |= HAL_MOD_ALT;
    return mods;
}

/*
 * Maps an SDL key to a HAL key.
 * Expects: an SDL keycode.
 * Returns: a HAL_KEY_* value (HAL_KEY_UNKNOWN if not used).
 */
static int _hal_key(SDL_Keycode k)
{
    switch (k) {
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:       return HAL_KEY_SHIFT;
    case SDLK_LCTRL:
    case SDLK_RCTRL:        return HAL_KEY_CTRL;
    case SDLK_LALT:
    case SDLK_RALT:         return HAL_KEY_ALT;
    case SDLK_BACKSPACE:    return HAL_KEY_BACKSPACE;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:     return HAL_KEY_RETURN;
    case SDLK_ESCAPE:       return HAL_KEY_ESCAPE;
    case SDLK_PLUS:
    case SDLK_EQUALS:
    case SDLK_KP_PLUS:      return HAL_KEY_PLUS;
    case SDLK_MINUS:
    case SDLK_KP_MINUS:     return HAL_KEY_MINUS;
    case SDLK_0:
    case SDLK_KP_0:         return HAL_KEY_0;
    }
    return HAL_KEY_UNKNOWN;
}

/*
 * Maps an SDL mouse button to a HAL button.
 * Expects: an SDL button number.
 * Returns: a HAL_MOUSE_* value, or 0 if not used.
 */
static int _hal_button(Uint8 b)
{
    if (b == SDL_BUTTON_LEFT)
        return HAL_MOUSE_LEFT;
    if (b == SDL_BUTTON_RIGHT)
        return HAL_MOUSE_RIGHT;
    if (b == SDL_BUTTON_MIDDLE)
        return HAL_MOUSE_MIDDLE;
    return 0;
}

/*
 * Copies a string, truncating it.
 * Expects: dst with room for len + 1 chars; src may be NULL.
 * Returns: nothing (dst is always '\0'-terminated).
 */
static void _hal_copy(char *dst, const char *src, size_t len)
{
    strncpy(dst, src ? src : "", len);
    dst[len] = '\0';
}
