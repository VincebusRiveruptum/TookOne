# TookOne

TookOne is a pixel-art bitmap and sequence (animation) editor inspired by the classic *Take ONE 4* (1992-2000).
It is being rebuilt from scratch in **ANSI C (C89)**, with its own immediate-mode interface built on
[microui](https://github.com/rxi/microui) and **SDL** as the first platform layer (Windows and Linux).
A native Windows backend may follow after the MVP.

Its file formats (`.bsf` sequences, `.bss` sprite sheets, `.bpk` game packages) and its UI code are also meant to
be reused by the BMPIO MS-DOS game engine.

## Layout

| Path | Contents |
|---|---|
| `app/` | Entry point, main loop, window layout, input routing |
| `core/std.h` | Common includes and fixed-size types |
| `core/image/` | Image model and BMP reader |
| `core/raster/` | Software drawing into an ARGB framebuffer |
| `ui/` | microui software renderer, bitmap font, custom controls |
| `hal/` | Platform contract (`hal.h`) |
| `platform/sdl/` | SDL3 implementation of `hal` |
| `deps/bsf/`, `deps/endian/` | Generic C89 deps (`.bsf` reader/writer, little-endian reading) |
| `external/microui/` | microui 2.02 (MIT), vendored unmodified |
| `docs/` | Requirements, Take ONE 4 analysis, architecture plan, format specs |
| `.agents/` | Code style, module hierarchy and checker scripts |

Module levels and allowed includes: [`.agents/rules/layers.md`](.agents/rules/layers.md).

## Building

Requirements: gcc, GNU make, SDL3 (found through `pkg-config sdl3`).

```sh
make -f makefile.linux              # build/linux/tookone
make -f makefile.linux test         # every STANDALONE test (no SDL needed)
./build/linux/tookone some.bmp      # or drop a BMP on the window
```

Zoom: `+` / `-` / `0`, Ctrl + mouse wheel, or the toolbar buttons.

MS-DOS (DOS/4G) build of the portable code (`deps/`, `core/`, `ui/`, microui) with OpenWatcom:

```sh
wmake -f makefile.watcom-linux build    # build/dos/took.lib + STANDALONE tests as .exe
```

Checks (see `.agents/skills/`):

```sh
python3 .agents/skills/c89-conformance/scripts/check_c89.py --target all   # C89, Linux and DOS
python3 .agents/skills/layer-check/scripts/check_layers.py                 # module hierarchy
```

## Documentation

- [Requirements](docs/base_reqs.md)
- [Take ONE 4 analysis](docs/take4_analisis.md)
- [Architecture plan](docs/arquitectura.md)
- [`.bsf` format](docs/bsf_format.md)
- [C code style](docs/c-code-stlye.md)
