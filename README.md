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
| `deps/bsf/` | C89 `.bsf` reader/writer, shared with BMPIO (gcc, clang, MSVC, OpenWatcom) |
| `external/microui/` | microui 2.02 (MIT), vendored unmodified |
| `docs/` | Requirements, Take ONE 4 analysis, architecture plan, format specs, C code style |

Planned: `core/`, `editor/`, `ui/`, `platform/`, `app/` (see the [architecture plan](docs/arquitectura.md)).

## Building

```sh
make -C deps/bsf test       # builds and runs the STANDALONE tests (gcc)
```

For MS-DOS (DOS/4G) with OpenWatcom:

```sh
cd deps/bsf && wmake -f makefile.wat     # bsf.obj + bsftest.exe
```

## Documentation

- [Requirements](docs/base_reqs.md)
- [Take ONE 4 analysis](docs/take4_analisis.md)
- [Architecture plan](docs/arquitectura.md)
- [`.bsf` format](docs/bsf_format.md)
- [C code style](docs/c-code-stlye.md)
