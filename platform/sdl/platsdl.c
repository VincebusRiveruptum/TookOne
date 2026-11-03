/*
 * platsdl - SDL3 backend of the platform layer.
 *
 * SDL3 headers are not C89, so this file is built with -std=c99
 * (see c-code-stlye.md, scope). The code itself follows the C89 style.
 */
#include <string.h>
#include <SDL3/SDL.h>
#include "../plat.h"

static SDL_Window *_window = NULL;
static SDL_Renderer *_renderer = NULL;
static SDL_Texture *_texture = NULL;
static int _texWidth = 0;
static int _texHeight = 0;

int plat_init(const char *title, int width, int height)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return PLAT_ERR_INIT;

    _window = SDL_CreateWindow(
        title,
        width,
        height,
        SDL_WINDOW_RESIZABLE
    );
    if (!_window) {
        SDL_Quit();
        return PLAT_ERR_INIT;
    }
    _renderer = SDL_CreateRenderer(_window, NULL);
    if (!_renderer) {
        SDL_DestroyWindow(_window);
        _window = NULL;
        SDL_Quit();
        return PLAT_ERR_INIT;
    }
    SDL_StartTextInput(_window);
    return PLAT_OK;
}

void plat_shutdown(void)
{
    if (_texture)
        SDL_DestroyTexture(_texture);
    if (_renderer)
        SDL_DestroyRenderer(_renderer);
    if (_window)
        SDL_DestroyWindow(_window);
    _texture = NULL;
    _renderer = NULL;
    _window = NULL;
    SDL_Quit();
}

void plat_windowSize(int *width, int *height)
{
    SDL_GetWindowSize(_window, width, height);
}

void plat_setTitle(const char *title)
{
    SDL_SetWindowTitle(_window, title);
}

unsigned long plat_ticksMs(void)
{
    return (unsigned long)SDL_GetTicks();
}

static int _mods(void)
{
    int mods = 0;
    SDL_Keymod m;

    m = SDL_GetModState();
    if (m & SDL_KMOD_SHIFT)
        mods |= PLAT_MOD_SHIFT;
    if (m & SDL_KMOD_CTRL)
        mods |= PLAT_MOD_CTRL;
    if (m & SDL_KMOD_ALT)
        mods |= PLAT_MOD_ALT;
    return mods;
}

static int _key(SDL_Keycode k)
{
    switch (k) {
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:       return PLAT_KEY_SHIFT;
    case SDLK_LCTRL:
    case SDLK_RCTRL:        return PLAT_KEY_CTRL;
    case SDLK_LALT:
    case SDLK_RALT:         return PLAT_KEY_ALT;
    case SDLK_BACKSPACE:    return PLAT_KEY_BACKSPACE;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:     return PLAT_KEY_RETURN;
    case SDLK_ESCAPE:       return PLAT_KEY_ESCAPE;
    case SDLK_PLUS:
    case SDLK_EQUALS:
    case SDLK_KP_PLUS:      return PLAT_KEY_PLUS;
    case SDLK_MINUS:
    case SDLK_KP_MINUS:     return PLAT_KEY_MINUS;
    case SDLK_0:
    case SDLK_KP_0:         return PLAT_KEY_0;
    }
    return PLAT_KEY_UNKNOWN;
}

static int _button(Uint8 b)
{
    if (b == SDL_BUTTON_LEFT)
        return PLAT_MOUSE_LEFT;
    if (b == SDL_BUTTON_RIGHT)
        return PLAT_MOUSE_RIGHT;
    if (b == SDL_BUTTON_MIDDLE)
        return PLAT_MOUSE_MIDDLE;
    return 0;
}

static void _copy(char *dst, const char *src, size_t len)
{
    strncpy(dst, src ? src : "", len);
    dst[len] = '\0';
}

/* Translates one SDL event. Returns 0 for events TookOne ignores. */
static int _translate(const SDL_Event *e, PlatEvent *ev)
{
    memset(ev, 0, sizeof(*ev));
    ev->mods = _mods();

    switch (e->type) {
    case SDL_EVENT_QUIT:
        ev->type = PLAT_EV_QUIT;
        return 1;
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        ev->type = PLAT_EV_RESIZE;
        SDL_GetWindowSize(_window, &ev->width, &ev->height);
        return 1;
    case SDL_EVENT_WINDOW_EXPOSED:
        ev->type = PLAT_EV_EXPOSE;
        return 1;
    case SDL_EVENT_MOUSE_MOTION:
        ev->type = PLAT_EV_MOUSEMOVE;
        ev->x = (int)e->motion.x;
        ev->y = (int)e->motion.y;
        return 1;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        ev->type = e->type == SDL_EVENT_MOUSE_BUTTON_DOWN
            ? PLAT_EV_MOUSEDOWN
            : PLAT_EV_MOUSEUP;
        ev->x = (int)e->button.x;
        ev->y = (int)e->button.y;
        ev->button = _button(e->button.button);
        return ev->button != 0;
    case SDL_EVENT_MOUSE_WHEEL:
        if (e->wheel.y == 0)
            return 0;
        ev->type = PLAT_EV_WHEEL;
        ev->wheel = e->wheel.y > 0 ? 1 : -1;
        return 1;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        ev->type = e->type == SDL_EVENT_KEY_DOWN
            ? PLAT_EV_KEYDOWN
            : PLAT_EV_KEYUP;
        ev->key = _key(e->key.key);
        return ev->key != PLAT_KEY_UNKNOWN;
    case SDL_EVENT_TEXT_INPUT:
        ev->type = PLAT_EV_TEXT;
        _copy(ev->text, e->text.text, PLAT_TEXT_LEN);
        return 1;
    case SDL_EVENT_DROP_FILE:
        ev->type = PLAT_EV_DROPFILE;
        _copy(ev->path, e->drop.data, PLAT_PATH_LEN);
        return 1;
    }
    return 0;
}

int plat_waitEvent(PlatEvent *ev, int timeoutMs)
{
    SDL_Event e;

    for (;;) {
        if (!SDL_WaitEventTimeout(&e, timeoutMs))
            return 0;
        if (_translate(&e, ev))
            return 1;
        /* Ignored event: keep waiting, but without blocking again
         * longer than requested. */
        if (timeoutMs == 0)
            return 0;
    }
}

static int _ensureTexture(int width, int height)
{
    if (_texture && _texWidth == width && _texHeight == height)
        return 1;
    if (_texture)
        SDL_DestroyTexture(_texture);
    _texture = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );
    if (!_texture)
        return 0;
    SDL_SetTextureScaleMode(_texture, SDL_SCALEMODE_NEAREST);
    _texWidth = width;
    _texHeight = height;
    return 1;
}

void plat_present(const void *argb, int width, int height)
{
    if (!_ensureTexture(width, height))
        return;
    SDL_UpdateTexture(
        _texture,
        NULL,
        argb,
        width * 4
    );
    SDL_RenderClear(_renderer);
    SDL_RenderTexture(
        _renderer,
        _texture,
        NULL,
        NULL
    );
    SDL_RenderPresent(_renderer);
}
