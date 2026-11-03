---
trigger: always_on
---

This rule defines the code style for my C projects.

- C89 Standard: This project is aimed for a 90's style programming which in that era evrything was following the C89 standard.
- Function naming scheme by module: Function names has a specific structure 'moduleabreviation_functionName' where the name is in camelCase,

For example: If the function resides in the 'VIDEO' module, then the function name will be "vid_putPixel".

- Early error validation for aviding heavility nested if blocks, for example: if (! videoMemory) return 0;
- Code per lines should not exceed 80 columns.
- Functions calls with more than 3 arguments, each one must be on it's own line.
- Variable always declared on top on the function, ordered by type hierarchy starting by primitives, each type along with the unsigned types, such as int, unsigned int, char, unsigned char. Then bool and Structs and typedef'd stucts at the end.
- Following the previous rule, pointer types should always be NULL-intialized on the top of the function.
- For now, there are two valid aproach for defining constants, by #define and by enums, most of the project already uses the first method, but in the future we must adopt using enums for better maintenance.

## Name conventions

- Camel case only on module variable and function names, example: counterIndex, matchAccomulator, etc.
- Snake case only used on the prefix on the variable/function module name part only when it's exportable/global, indicating the namespace. For example: ed_matchAccomulator., vid_bufferIndex, etc.
- Private functions must use the '_' prefix, also, they must be strictly used in-module only.
- Constants and #define always in uppercase, also must follow the module prefix rule, however fully in Snake Case. For example: ED_MAX_BUFFER_LENGTH

## Files

- Files/Paths names should be all lowercase, this is for compatibility between linux and DOS.
- Files/Path names length should not exceed 8 characters, this is for compatibility between linux and DOS.

## Dependencies (DEPS folder)

If a functionality is independent of platform and fully portable, then it be reused in the future by moving it to the "deps" folder, this will be a contribution to my personal C framework. If the domain is not generic, then it must stay in the "core" folder. 

Also, all deps should ideally be standalone, this means that could be tested independently. However, there are some cases that some have cross-references, this could be problematic as maintanace
gets complex, so avoid doing this. 

- The #ifdef STANDALONE specifies the entry main function and the test code.
- The dep module should have generic types, avoid using such as bool that could be cross referenced.
- Function names must not have prefix unlike core module functions.
- Each dep must be written portable as possible.

## OBOE avoiding conventions

For avoiding OBOE type errors, we need to do the following:

1. Incrementing for loops always as [0, n): for(i=0; i < n ; i++)
2. Decrementing for loops always as: for(i=n-1 ; i >= 0 ; i--)
3. Length =/= Last index :  Last index is Length - 1;
4. Semi-open range convention [start, end):

'''c
// [0, n) means: 0, 1, 2, ..., n-1
void substring(char *str, int start, int end) {
    // Copy chars from start to end-1
    for (int i = start; i < end; i++) {
        putchar(str[i]);
    }
}
'''

5. String length is always real useful chars count, it does not consider null-terminator, this is the C standard convention, for example, strlen() follows this scheme.

6. Static buffer string length: Strings size is set by a constant which also follow the
rule of useful real characters, however we need to always add null character to the size constant. 

'''c
#define MAX_LINE_LEN 256

typedef struct {
    char buffer[MAX_LINE_LEN + 1];  // +1 for NULL
    int length;  // DOES NOT INCLUDE NULL
} Line;

// Invariant: buffer[length] == '\0' ALWAYS
// buffer[0..length-1] are valid chars.

void append_char(Line *line, char ch) {
    if (line->length < MAX_LINE_LEN) {
        line->buffer[line->length++] = ch;
        line->buffer[line->length] = '\0';  // keep invariant
    }
}

void process_line(Line *line) {
    for (int i = 0; i < line->length; i++) {
        printf("%c", line->buffer[i]);
    }
}
'''

NOTE: We need to improve the dependency management so each one are fully isolated from the rest, avoiding cross-references.

## Function description above definition

All functions should have a brief description above the function header, it should explain what it does but NOT
how, what expects and what returns. Nothing else. It goes above the **definition** (`.c`), for public and private
functions alike; headers only keep short group comments ("Lifecycle", "I/O", ...).

'''c
/*
 * Loads a BMP file into img.
 * Expects: a readable path and an Img to fill.
 * Returns: BMP_OK, or a BMP_ERR_* code (img is left empty).
 */
int bmp_load(const char *path, Img *img)
'''

## Error handling and logging

Logging is aimed at the programmer: simple to write and informative enough to debug. Three levels:

- **Info** (`log_info`): plain informative message.
- **Warning** (`log_warn`): recoverable condition that **can happen with real data**, even if the code is correct:
  missing or malformed file, not enough memory for an image whose size the user chose, etc. It logs and the
  function returns an error value or NULL.
- **Error** (`log_error`): a **bug** (broken invariant, impossible state, index out of range that "cannot happen",
  failure of a small fixed-size allocation at startup). It logs and ends with `exit(1)`. It must never be caused by
  user input or by the environment: those are warnings. Memory errors are not automatically errors; decide by
  the cause.

Rules:

- Messages start with the function name between brackets: `log_warn("[bmp_load] cannot open %s", path);`
  C89 has no `__func__` nor variadic macros, so the name is written by hand.
- Release builds (no `-DDEBUG`): `log_info` is disabled; `log_warn` and `log_error` stay, `log_error` still ends the
  program (stopping is safer than going on with a corrupted state). Messages go to the log file.
- **Deps never log nor exit.** They return error codes (`BSF_ERR_*`) and the caller decides the level and logs.
  This keeps deps free of cross-references (e.g. to `deps/log`) and lets other programs decide how to fail.
- Messages for the **end user** (status bar, dialogs) are a separate concern, handled in a centralized way (TBD).

## Modules by responsibility

Each module is a folder with one responsibility:

'''
core/image/
|- image.h        public API: the only header other modules may include
|- private.h      types / functions shared by the module's own files; never included from outside,
|                 never included from image.h
|- img_base.c     one file per responsibility, named <prefix>_<responsibility>.c (8.3)
|- img_bmp.c
'''

- Modules talk to each other following the hierarchy defined in [`layers.md`](layers.md): a module only includes
  and calls modules of a **lower** level; modules of the same level never call each other (the upper level
  coordinates them).
- Private functions: `_<prefix>_name`, both `static` file helpers and module-internal functions declared in
  `private.h` (e.g. `_img_readFile`). What tells them apart is where they are declared.
- State lives in structs passed by pointer. Globals only inside a module and `static`; no `extern` state shared
  between modules.
- Shared definitions for the whole project (common includes, `bool`, fixed-size types) live in `core/std.h`.

## File layout: outside-in

Inside each `.c` file, functions go from the outside to the inside: the lower a function is in the file, the less
likely it is called from somewhere else.

'''c
/*
 * img_bmp - BMP reader / writer (module: image).
 */
#include "image.h"
#include "private.h"

/* ---- Prototypes of file-private functions ---- */
static int _img_readFile(const char *path, TkU8 **out, long *size);

/* ================================================================
 * 1. Public API: callable from other modules (declared in image.h)
 * ================================================================ */

/* ================================================================
 * 2. Module internal: shared by this module's files (private.h)
 * ================================================================ */

/* ================================================================
 * 3. File private: helpers used only here (static)
 * ================================================================ */

#ifdef STANDALONE
/* ================================================================
 * 4. Tests: built with -DSTANDALONE as a test program
 * ================================================================ */
int main(void) { ... }
#endif
'''

- In C89 a `static` function defined after its caller needs a prototype first (otherwise the compiler assumes an
  implicit declaration and then clashes with `static`). That is why the file starts with the block of private
  prototypes; it also works as the index of the file.
- Tests: every file ends with its `#ifdef STANDALONE` block, in deps **and** in core modules. The makefile builds
  each one with `-DSTANDALONE`, linking the rest of its module.

## Scope

This guide applies to **all of our own C code**:

| Folder | Applies? |
|---|---|
| `deps/` | Yes, fully (including *Dependencies*) |
| `core/`, `editor/`, `ui/`, `hal/`, `app/` | Yes, fully |
| `platform/<backend>/` | Yes, with one exception: system headers (SDL, `windows.h`) are not strict C89, so the backend may be built with more permissive flags. Its own code still follows the guide |
| `external/` | No. Third-party code (e.g. microui) is kept **unmodified**, with its own style, so it can be updated and fixes contributed upstream |
| Build files (`.github/`) and `docs/` | No |

### Exception: names in shared deps

Deps have no module prefix. In deps that are linked together with other libraries (SDL, microui, each platform's
libc...) a generic name can clash with another symbol (e.g. `crc32` from zlib). In those deps public functions
start with the **dep name in camelCase, without underscore**: `bsfRead`, `bsfWrite`, `bsfInit`. Types use
PascalCase with the same name: `BsfSequence`, `BsfFrame`. Constants: `BSF_*`.
