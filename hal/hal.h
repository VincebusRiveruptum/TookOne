/*
 * hal - platform contract (level 4, used only by app).
 *
 * Each backend in platform/<name>/ implements these functions; nothing
 * else in TookOne talks to the operating system.
 */
#ifndef HAL_HAL_H
#define HAL_HAL_H

#define HAL_PATH_LEN        511     /* useful chars of HalEvent.path */
#define HAL_TEXT_LEN        31      /* useful chars of HalEvent.text */

enum {
    HAL_OK = 0,
    HAL_ERR_INIT
};

/* Event types */
enum {
    HAL_EV_NONE = 0,
    HAL_EV_QUIT,
    HAL_EV_RESIZE,          /* width, height */
    HAL_EV_EXPOSE,          /* window needs a redraw */
    HAL_EV_MOUSEMOVE,       /* x, y */
    HAL_EV_MOUSEDOWN,       /* x, y, button */
    HAL_EV_MOUSEUP,         /* x, y, button */
    HAL_EV_WHEEL,           /* wheel: +1 up, -1 down */
    HAL_EV_KEYDOWN,         /* key */
    HAL_EV_KEYUP,           /* key */
    HAL_EV_TEXT,            /* text (UTF-8) */
    HAL_EV_DROPFILE         /* path */
};

/* Mouse buttons */
enum {
    HAL_MOUSE_LEFT = 1,
    HAL_MOUSE_RIGHT,
    HAL_MOUSE_MIDDLE
};

/* Keys the application cares about */
enum {
    HAL_KEY_UNKNOWN = 0,
    HAL_KEY_SHIFT,
    HAL_KEY_CTRL,
    HAL_KEY_ALT,
    HAL_KEY_BACKSPACE,
    HAL_KEY_RETURN,
    HAL_KEY_ESCAPE,
    HAL_KEY_PLUS,
    HAL_KEY_MINUS,
    HAL_KEY_0
};

/* Modifier flags of HalEvent.mods */
enum {
    HAL_MOD_SHIFT = 0x01,
    HAL_MOD_CTRL = 0x02,
    HAL_MOD_ALT = 0x04
};

typedef struct HalEvent {
    int type;
    int x;
    int y;
    int width;
    int height;
    int button;
    int wheel;
    int key;
    int mods;
    char text[HAL_TEXT_LEN + 1];
    char path[HAL_PATH_LEN + 1];
} HalEvent;

/* ---- Lifecycle ---- */

int hal_init(const char *title, int width, int height);
void hal_shutdown(void);

/* ---- Window ---- */

void hal_windowSize(int *width, int *height);
void hal_setTitle(const char *title);
void hal_present(const void *argb, int width, int height);

/* ---- Events and time ---- */

int hal_waitEvent(HalEvent *ev, int timeoutMs);
unsigned long hal_ticksMs(void);

#endif /* HAL_HAL_H */
