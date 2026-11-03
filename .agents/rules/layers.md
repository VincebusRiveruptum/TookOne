---
trigger: always_on
---

This rule defines the module hierarchy of TookOne: which module may include (and call) which.
It complements `c-code-stlye.md` (section *Modules by responsibility*).

## Rules

1. A module may only include and call modules of a **lower** level.
2. Modules of the **same level never call each other** (siblings do not know each other). When two siblings must
   cooperate, the level above coordinates them. Example: the canvas (`ui`) does not call the pencil (`editor`);
   `app` receives the canvas event and passes it to the editor.
3. Inside a module, its own files may include each other's headers freely (`<module>.h` and `private.h`).
4. `private.h` of a module is never included from outside the module.
5. Deps are level 0 and do not use each other (each dep is self-contained), see `c-code-stlye.md`.
6. Platform backends (`platform/<name>/`) implement the `hal` contract. They may include `hal` and levels 0-1,
   and **nobody includes them**: they are only linked.
7. System headers (`<...>`) are not restricted by this rule.

Changing a level or adding a module is done **here**, in the table below. The checker reads it.

## Levels

| Level | Module | Path | Notes |
|---|---|---|---|
| 0 | each dep | `deps/<name>/` | generic, self-contained, shared with BMPIO |
| 0 | each external library | `external/<name>/` | third party, unmodified |
| 1 | std | `core/std.h` | common includes, `bool`, fixed-size types |
| 2 | image | `core/image/` | image model, palette, BMP codec |
| 3 | raster | `core/raster/` | software drawing into a framebuffer (draws images) |
| 4 | editor | `editor/` | document, tools, undo, viewport |
| 4 | ui | `ui/` | microui renderer, font, controls |
| 4 | hal | `hal/` | platform contract (only `app` uses it) |
| 5 | app | `app/` | entry point; coordinates `editor`, `ui` and `hal` |
| P | platform backends | `platform/<name>/` | implement `hal`; never included |

Machine-readable copy of the table, used by `check_layers.py`
(`*` = every direct subfolder is a separate module; `P` = platform backend):

```layers
0  deps/*
0  external/*
1  core/std.h
2  core/image
3  core/raster
4  editor
4  ui
4  hal
5  app
P  platform/*
```

## Checking

```bash
python3 .agents/skills/layer-check/scripts/check_layers.py
```

It parses every `#include "..."` of our own C files, resolves it to a module and reports:

- includes of a module of the same or a higher level,
- includes of another module's `private.h`,
- includes of a platform backend,
- C files that do not belong to any module of the table (so the table stays complete).
