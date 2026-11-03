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
| `app/` | Entry point and main loop (`tookone.c`) |
| `core/` | Image model, BMP reader, software rasterizer (C89, libc only) |
| `ui/` | microui software renderer, bitmap font, custom controls |
| `platform/` | Platform contract (`plat.h`) and the SDL3 backend |
| `deps/bsf/` | `.bsf` reader/writer, shared with BMPIO |
| `external/microui/` | microui 2.02 (MIT), vendored unmodified |
| `tests/` | Console tests |
| `docs/` | Requirements, Take ONE 4 analysis, architecture plan, format specs, C code style |

## Building

Requirements: gcc, GNU make, SDL3 (found through `pkg-config sdl3`).

```sh
make -f makefile.linux              # build/linux/tookone
make -f makefile.linux test         # console tests (no SDL needed)
./build/linux/tookone some.bmp      # or drop a BMP on the window
```

Zoom: `+` / `-` / `0`, Ctrl + mouse wheel, or the toolbar buttons.

MS-DOS (DOS/4G) build of the portable code (`deps/`, `core/`, `ui/`, microui) with OpenWatcom:

```sh
wmake -f makefile.watcom-linux build    # build/dos/took.lib + test executables
```

C89 conformance of both targets (see `.agents/skills/c89-conformance`):

```sh
python3 .agents/skills/c89-conformance/scripts/check_c89.py --target all
```

## Documentation

- [Requirements](docs/base_reqs.md)
- [Take ONE 4 analysis](docs/take4_analisis.md)
- [Architecture plan](docs/arquitectura.md)
- [`.bsf` format](docs/bsf_format.md)
- [C code style](docs/c-code-stlye.md)
