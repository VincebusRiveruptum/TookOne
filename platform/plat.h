/*
 * plat - platform layer contract. Each backend (sdl/, win32/, dos/)
 * implements these functions; nothing else in TookOne talks to the OS.
 */
#ifndef PLAT_H
#define PLAT_H

#define PLAT_PATH_LEN       511     /* useful chars, see PlatEvent.path */
#define PLAT_TEXT_LEN       31

enum {
    PLAT_OK = 0,
    PLAT_ERR_INIT
};

/* Event types */
enum {
    PLAT_EV_NONE = 0,
    PLAT_EV_QUIT,
    PLAT_EV_RESIZE,         /* width, height */
    PLAT_EV_EXPOSE,         /* window needs a redraw */
    PLAT_EV_MOUSEMOVE,      /* x, y */
    PLAT_EV_MOUSEDOWN,      /* x, y, button */
    PLAT_EV_MOUSEUP,        /* x, y, button */
    PLAT_EV_WHEEL,          /* wheel: +1 up, -1 down */
    PLAT_EV_KEYDOWN,        /* key */
    PLAT_EV_KEYUP,          /* key */
    PLAT_EV_TEXT,           /* text (UTF-8) */
    PLAT_EV_DROPFILE        /* path */
};

/* Mouse buttons */
enum {
    PLAT_MOUSE_LEFT = 1,
    PLAT_MOUSE_RIGHT,
    PLAT_MOUSE_MIDDLE
};

/* Keys the application cares about */
enum {
    PLAT_KEY_UNKNOWN = 0,
    PLAT_KEY_SHIFT,
    PLAT_KEY_CTRL,
    PLAT_KEY_ALT,
    PLAT_KEY_BACKSPACE,
    PLAT_KEY_RETURN,
    PLAT_KEY_ESCAPE,
    PLAT_KEY_PLUS,
    PLAT_KEY_MINUS,
    PLAT_KEY_0
};

/* Modifier flags in PlatEvent.mods */
enum {
    PLAT_MOD_SHIFT = 0x01,
    PLAT_MOD_CTRL = 0x02,
    PLAT_MOD_ALT = 0x04
};

typedef struct PlatEvent {
    int type;
    int x;
    int y;
    int width;
    int height;
    int button;
    int wheel;
    int key;
    int mods;
    char text[PLAT_TEXT_LEN + 1];
    char path[PLAT_PATH_LEN + 1];
} PlatEvent;

int plat_init(const char *title, int width, int height);
void plat_shutdown(void);

/* Current drawable size in pixels. */
void plat_windowSize(int *width, int *height);

/*
 * Waits up to timeoutMs (-1 = forever) for an event the application
 * understands. Returns 1 and fills *ev, or 0 on timeout.
 */
int plat_waitEvent(PlatEvent *ev, int timeoutMs);

/* Shows a 32-bit 0xAARRGGBB framebuffer, width * height pixels. */
void plat_present(const void *argb, int width, int height);

void plat_setTitle(const char *title);

unsigned long plat_ticksMs(void);

#endif /* PLAT_H */
