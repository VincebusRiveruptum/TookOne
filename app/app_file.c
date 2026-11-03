/*
 * app_file - opening images and saving screenshots (module: app).
 */
#include "private.h"

/* ---- Prototypes of file-private functions ---- */
static void _app_baseName(const char *path, char *out);

/* ================================================================
 * 2. Module internal (declared in private.h)
 * ================================================================ */

/*
 * Loads a BMP as the current image and shows its info in the status.
 * Expects: an App and a path; on error the current image is kept.
 * Returns: nothing (the result is shown in the status bar).
 */
void _app_loadFile(App *app, const char *path)
{
    int err;
    char name[APP_NAME_LEN + 1];
    Img img;

    _app_baseName(path, name);
    err = img_loadBmp(path, &img);
    if (err != IMG_OK) {
        sprintf(
            app->status,
            "%s: %s",
            name,
            img_strError(err)
        );
        return;
    }

    img_free(&app->img);
    app->img = img;
    app->hasImage = 1;
    ras_buildLut(&app->img, app->lut);
    app->view.img = &app->img;
    app->view.lut = app->lut;
    _app_fitZoom(app);

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
    hal_setTitle(app->title);
}

/*
 * Saves a framebuffer as a binary PPM (P6) file.
 * Expects: a framebuffer and the output path.
 * Returns: 1 on success, 0 if the file cannot be written.
 */
int _app_writePpm(const RasFb *fb, const char *path)
{
    int i;
    int count;
    TkU32 px;
    TkU8 rgb[3];
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
        rgb[0] = (TkU8)((px >> 16) & 0xFF);
        rgb[1] = (TkU8)((px >> 8) & 0xFF);
        rgb[2] = (TkU8)(px & 0xFF);
        fwrite(
            rgb,
            1,
            3,
            fp
        );
    }
    return fclose(fp) == 0;
}

/* ================================================================
 * 3. File private
 * ================================================================ */

/*
 * Extracts the file name from a path, truncated to APP_NAME_LEN chars.
 * Expects: a path and out with room for APP_NAME_LEN + 1 chars.
 * Returns: nothing.
 */
static void _app_baseName(const char *path, char *out)
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
