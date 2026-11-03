# TookOne — Plan de arquitectura (paso 2)

Objetivo: un editor de escritorio **portable, minimalista, simple, eficiente y resiliente**, **reconstruido desde cero
en ANSI C (C89)**, con **interfaz propia** sobre **microui** y **SDL** como primera capa de plataforma (Windows y Linux).
Si el MVP funciona, se podrá escribir un backend **nativo de Windows** (Win32/GDI) sin tocar el resto.

La misma interfaz y los mismos formatos se reutilizan en el motor **BMPIO** (C89, OpenWatcom, MS-DOS 32-bit, Mode X).

Documentos relacionados: [`arch.md`](arch.md) (exploración de opciones) · [`c-code-stlye.md`](c-code-stlye.md) (estilo C) ·
[`base_reqs.md`](base_reqs.md) · [`bsf_format.md`](bsf_format.md) · [`take4_analisis.md`](take4_analisis.md)

> Esta decisión **reemplaza** la de `arch.md` §9 (Qt + C++). En términos de `arch.md` §3 es la **opción C**:
> interfaz propia dibujada sobre una capa de plataforma mínima.

---

## 1. Decisiones tomadas

| # | Decisión | Estado |
|---|---|---|
| 1 | **Todo en C89** siguiendo `c-code-stlye.md`. Sin C++ | Confirmado |
| 2 | **Interfaz propia** en modo inmediato sobre **microui** (MIT, ~1200 líneas, sin `malloc`), sin modificar, en `external/microui/` | Confirmado |
| 3 | **SDL3** como primer backend de plataforma (Windows + Linux). Backend Win32 nativo después del MVP | Confirmado |
| 4 | Por qué no Qt / GTK / libui-ng: priorizar portabilidad, minimalismo, C puro y reutilización en BMPIO. Se acepta perder la apariencia nativa | Confirmado |
| 5 | Librería de imágenes: **sin ImageMagick**; codecs propios (BMP, PAL, BSF) | Confirmado |
| 6 | Extensiones: `.bsf` (secuencia), `.bss` (sprite sheet), `.bpk` (paquete) | Confirmado |
| 7 | Datos de juego en **texto plano propio** (por ahora `KEY=VALUE`), nada de JSON | Confirmado (sintaxis por definir) |
| 8 | Compresión de frames: **RLE obligatoria**, deflate opcional/futura | Confirmado |
| 9 | Lector/escritor `.bsf` como **dep genérica** `deps/bsf/` | Confirmado |
| 10 | **Build con make**: `makefile.linux` (gcc) y `makefile.watcom-linux` (OpenWatcom, objetivo `build`), verificados con la skill `.agents/skills/c89-conformance`. Sin CMake | Confirmado |
| 11 | Método: **vertical slices** (§9), no spec-driven. Las deps genéricas se crean o integran cuando un slice las necesita | Confirmado |

## 2. Capas

Regla de oro: **las dependencias apuntan hacia adentro**. Solo `platform/` conoce SDL (o Win32, o el hardware VGA).

La jerarquía oficial de módulos (niveles, quién puede incluir a quién) está en
[`.agents/rules/layers.md`](../.agents/rules/layers.md) y se verifica con
`.agents/skills/layer-check/scripts/check_layers.py`. El diagrama de abajo es una vista general.

```
┌──────────────────────────────────────────────────────────────────┐
│  app/        main + bucle: une plataforma, UI y editor            │
├──────────────────────────────────────────────────────────────────┤
│  hal/ +      hal_*   — ventana, eventos, presentar framebuffer,    │  platform/sdl/  (primero)
│  platform/   tiempo, portapapeles, diálogos de archivo             │  platform/win32/ (post-MVP)
│                                                                    │  platform/dos/ (BMPIO: Mode X)
├──────────────────────────────────────────────────────────────────┤
│  ui/         controles propios sobre microui + renderer por        │  menú, toolbar, paleta, film strip,
│              software que pinta los comandos de microui            │  canvas, diálogos; fuente bitmap
├──────────────────────────────────────────────────────────────────┤
│  editor/     documento, herramientas, comandos/undo, viewport      │  testeable sin ventana
├──────────────────────────────────────────────────────────────────┤
│  core/       imagen, paleta, raster, transformaciones, color,      │  solo libc
│              render, codecs BMP/PAL                                │
├──────────────────────────────────────────────────────────────────┤
│  deps/       genéricas y autocontenidas (bsf, …)                   │  sin prefijo, STANDALONE tests
│  external/   código de terceros sin modificar (microui)            │
└──────────────────────────────────────────────────────────────────┘
```

### 2.1 La clave: todo se dibuja en un framebuffer propio

La UI **no dibuja con SDL**. microui genera comandos (rectángulo, texto, ícono, recorte) y `ui/` los **rasteriza por software**
en un framebuffer en memoria, reutilizando `core/` (rectángulos, blit de glifos de una fuente bitmap). La plataforma solo
**presenta** ese framebuffer:

```
editor + ui ──► microui (comandos) ──► ui/ui_rend (software) ──► framebuffer ──► hal_present()
                                                                                    ├─ SDL: textura streaming
                                                                                    ├─ Win32: StretchDIBits
                                                                                    └─ DOS: copia a VGA (Mode X)
```

Consecuencias:

- **Portar = escribir un backend de `platform/`** (unas pocas funciones). La UI, el editor y el núcleo no cambian.
- **La misma UI corre en BMPIO**: microui compila con OpenWatcom para DOS/4G sin advertencias (verificado; falta probarlo
  en ejecución). Un editor de niveles o panel de debug dentro del juego usaría exactamente este código.
- **Píxeles exactos**: el canvas del editor y la UI comparten el mismo framebuffer; zoom nearest-neighbor y grid sin depender
  de la GPU.
- **Eficiencia**: se redibuja solo cuando llega un evento (no a 60 fps como un juego), y se puede comparar la lista de
  comandos de microui con la del frame anterior para no redibujar si no cambió nada.

### 2.2 Interfaz de plataforma (`hal/`)

Contrato pequeño y explícito en `hal/hal.h` (prefijo `hal_`, como en dcode-ide). Lo implementa cada backend de
`platform/<nombre>/` y solo lo usa `app` (ver `.agents/rules/layers.md`):

```c
/* hal/hal.h (hoy) */
int  hal_init(const char *title, int width, int height);
void hal_shutdown(void);

void hal_windowSize(int *width, int *height);
void hal_setTitle(const char *title);
void hal_present(const void *argb, int width, int height);

int  hal_waitEvent(HalEvent *ev, int timeoutMs);    /* 0 = sin evento */
unsigned long hal_ticksMs(void);

/* cuando un slice los necesite: diálogos de archivo, portapapeles */
```

`HalEvent` es un evento abstracto (mouse, teclado, texto, rueda, resize, cerrar, soltar archivo). Cada backend traduce
sus eventos (`SDL_Event`, `WM_*`, interrupciones de teclado/mouse en DOS) a este formato.

### 2.3 ¿Qué va en `deps/`, `core/`, `external/`?

- `deps/`: genérico, portable y útil fuera de TookOne (ej. `bsf`, que usa BMPIO). Cada dep es autocontenida (un `.c` + un `.h`).
- `core/<módulo>/`, `editor/`, `ui/`, `hal/`, `app/`: dominio de TookOne. Funciones con prefijo de módulo (`img_loadBmp`, `ras_fillRect`, `ui_image`, `hal_present`).
- `external/`: código de terceros tal cual (microui). No se modifica, para poder actualizarlo y contribuir arreglos al proyecto original.

Candidato a dep cuando madure: los **controles propios de `ui/`** que no dependan de TookOne (menú, toolbar, fuente bitmap,
renderer por software) podrían pasar a `deps/` para usarlos en BMPIO.

## 3. Estructura del repositorio

Estructura objetivo; las carpetas se crean a medida que un slice las necesita (§9).

```
TookOne/
├─ makefile.linux          gcc: tookone (SDL3); `test` corre todos los tests STANDALONE
├─ makefile.watcom-linux   OpenWatcom (`build`): deps + core + ui + microui → took.lib y tests para DOS
├─ deps/
│  ├─ bsf/                 bsf.c, bsf.h (lector/escritor .bsf, con makefiles propios)
│  └─ endian/              endian.c, endian.h (lectura little-endian)
├─ external/
│  └─ microui/             microui.c, microui.h, LICENSE, VERSION.md (sin modificar)
├─ core/
│  ├─ std.h                includes comunes y TkU8/TkU16/TkU32
│  ├─ image/               image.h, private.h, img_base.c, img_bmp.c
│  └─ raster/              raster.h, private.h, ras_base.c, ras_blit.c
├─ editor/                 (slice 2+) doc, tool, cmd (undo), view
├─ ui/                     ui.h, private.h, ui_rend.c, ui_font.c, ui_fdata.c
├─ hal/                    hal.h (contrato de plataforma)
├─ platform/
│  ├─ sdl/                 hal_sdl.c (Windows + Linux)
│  └─ win32/               (después del MVP)
├─ app/                    private.h, main.c, app_zoom.c, app_file.c, app_inp.c, app_ui.c
└─ docs/
```

Nombres de archivo en minúsculas y de máximo 8 caracteres, como pide la guía.

## 4. Núcleo (`core/`, `editor/`)

### 4.1 Modelo de datos

El documento es una **Secuencia** (un BMP es una secuencia de un frame), manejada con un **handle opaco**:

```c
/* editor/doc.h */
typedef struct Doc Doc;                     /* opaco */

Doc *doc_new(int width, int height, int bpp);
Doc *doc_fromBsf(const BsfSequence *seq);   /* importa desde deps/bsf */
void doc_free(Doc *doc);

int  doc_frameCount(const Doc *doc);
int  doc_addFrame(Doc *doc, int index);
int  doc_save(const Doc *doc, const char *path);
```

Internamente:

```
Doc
├─ width, height, bpp           (1, 4, 8, 16, 24)
├─ Palette                       (hasta 256 RGB; vacía en 16/24 bpp)
├─ transparent, defaultDelay, loop, loopCount, name
├─ frames[]                      Image (un píxel por byte en indexado; el empaquetado lo hacen los codecs) + delay
└─ undo                          historial de comandos

EditorState (no se guarda en .bsf)
├─ primary, secondary, transparent   (rotables por atajo)
├─ currentFrame, selection, onionSkin (prev/next/any, opacidad), grid
└─ activeTool, brush, ditherPattern
```

- **El color indexado se mantiene indexado**: rotación de paleta, LUTs y recolor de segmentos solo cambian la paleta.
- **Modo bitmap vs modo secuencia** es `doc_frameCount(doc) == 1` + preferencia de vista.

### 4.2 Herramientas, comandos, viewport

- Cada herramienta es una máquina de estados que recibe eventos abstractos en coordenadas de imagen y **devuelve el
  rectángulo modificado** (`Rect tool_move(Doc *doc, int x, int y)`), para redibujar solo esa región.
- Primitivas en `core/draw`: Bresenham, elipse por punto medio, rectángulo, **flood fill por scanlines**, con pincel y patrón de dither.
- **Undo** por comandos (`apply`/`revert`); los de dibujo guardan solo el rectángulo afectado. Límite en KB (como Take ONE).
- **Viewport** (`editor/view`): zoom/pan y conversión pantalla↔imagen.

### 4.3 Render y color

- `core/render`: frame indexado → LUT de paleta → ARGB, con onion skin. Color cycling = re-render con la LUT rotada.
- `core/color`: RGB↔HSL, cuantización (median cut / octree), dithering (Floyd-Steinberg, ordenado), remapeo, LUTs.

## 5. Interfaz (`ui/`)

microui aporta: ventanas internas, paneles con scroll, botón, checkbox, slider, campo numérico, label, texto, header,
árbol, popup y layout. TookOne agrega controles propios (en modo inmediato es una función que lee input y emite comandos):

| Control | Notas |
|---|---|
| `ui_rend` | Rasteriza los comandos de microui en el framebuffer (rect, texto, ícono, clip) e incluye el control de imagen |
| `font` | Fuente bitmap fija (estilo VGA / Take ONE); `text_width` / `text_height` para microui |
| Barra de menú + menús | Con popups de microui; atajos de teclado en una tabla central (base: atajos de Take ONE, `take4_analisis.md` §4.8) |
| Toolbar | Botones con íconos propios (dibujados por ti; no se usan recursos de Take ONE) |
| Canvas | Muestra el frame con zoom/grid/onion skin; envía eventos a `tool_*()` |
| Paleta | Grilla de colores; selección primario/secundario/transparente; panel acoplable a un lado o flotante |
| Film strip | Miniaturas a la derecha o abajo, selección múltiple, arrastrar para reordenar |
| Barra de reproducción | Play/pausa, anterior/siguiente, primero/último, loop, delay |
| Campo de texto | microui solo agrega y borra: extender con cursor y selección cuando haga falta (nombres de archivo) |
| Diálogos | Nuevo, cambiar formato, etc. como ventanas de microui. Abrir/guardar: ver §6 |

**Paneles acoplables**: layout propio simple (paneles anclados a izquierda/derecha/abajo con divisores arrastrables, o
flotantes como ventana de microui). Resuelve la crítica a Take ONE (paleta rígida) sin un sistema de docking completo.

## 6. Plataforma: SDL3 (elegido)

| | SDL2 | SDL3 |
|---|---|---|
| Estado | Rama anterior, en mantenimiento | Rama actual |
| Diálogos de archivo nativos | No (usar tinyfiledialogs o un explorador propio en microui) | Sí |
| Portapapeles de imágenes | Solo texto | Datos con tipo MIME |
| Windows antiguos | Llega más atrás | Más moderno |

Se eligió **SDL3** (3.4 en desarrollo; en Ubuntu 24.04 se compila desde el código fuente porque no está en `apt`).
Como solo `platform/sdl/` incluye SDL, cambiar de versión afecta a un único backend. Los headers de SDL no son C89 estricto,
así que ese backend se compila con flags algo más permisivos; el resto del código sigue en C89 estricto.

## 7. Formatos

Familia `"TOOK"` + tipo (ver `bsf_format.md`); cada nivel empotra al anterior sin conversión:

```
.bsf  Secuencia      "TOOK" "BSF "   chunks: HEAD, PLTE, NAME, FRAM×N, END
.bss  Sprite sheet   "TOOK" "SHT "   chunks: HEAD, PLTE (compartida, opcional), SEQ ×N, END   (SEQ = un .bsf completo)
.bpk  Paquete        "TOOK" "PAK "   cabecera + blobs + directorio de entradas de 48 bytes (estilo WAD)
```

| Formato | Lectura / escritura |
|---|---|
| BSF | `deps/bsf` |
| BSS / BPK | deps futuras (`bss` dependerá de `bsf`: excepción documentada) |
| BMP 1/4/8/16/24 | `core/image` (`img_bmp.c`, control exacto de paleta) |
| JASC PAL | `core/jascpal` |
| PNG / GIF | Post-MVP: decodificador/encoder propio o single-header (p.ej. `stb_image` para leer) en `external/` |

### 7.1 Paquete `.bpk` (borrador)

```
Header (16 bytes):   char magic[8] = "TOOK" "PAK "; u32 entry_count; u32 dir_offset
Blobs (alineados a 4 bytes)
Directorio: entry_count × 48 bytes
  u32 offset; u32 size; char type[4] ("SHT ", "BSF ", "TEXT", "MAP ", "PAL ", "RAW "); u32 flags; char name[32]
```

**TEXT**: datos de juego en texto plano propio (`KEY=VALUE`, `#` comentarios, como los `.cfg` de BMPIO). **MAP**: por definir.

## 8. Build y pruebas

- `make` (gcc) en Windows (MinGW) y Linux; `wmake -f makefile.wat` para lo que corre en DOS.
- Tests de consola en cada módulo C (`#ifdef STANDALONE` en deps; `make test` en core/editor), con AddressSanitizer/UBSan en gcc.
- Pruebas de UI sin ventana: renderizar un frame de la UI al framebuffer y compararlo con un BMP esperado.
- CI (GitHub Actions): `make test` en Linux y Windows (MinGW); build Watcom de las deps.

## 9. Método: vertical slices

El desarrollo avanza por **slices verticales**, no por capas ni por especificación previa. Cada slice entrega **una
funcionalidad completa que se puede usar y probar en la app**, atravesando todas las capas que necesite
(`platform` → `ui` → `editor` → `core` → `deps`), y solo lo mínimo de cada una.

- **Las capas (§2) dicen dónde va el código; los slices dicen en qué orden se escribe.**
- **Nada se construye "por si acaso"**: un módulo, control o dep genérica nace cuando un slice lo necesita.
  Las deps de `deps/` traídas de BMPIO se integran (y se ajustan a la guía) en el slice que las use.
- **Definición de terminado** de un slice:
  1. Se puede hacer en la app (Linux; Windows cuando el backend esté disponible).
  2. La lógica nueva de `core/`, `editor/` o `deps/` tiene tests de consola.
  3. Compila sin advertencias en C89 estricto (gcc) y, si el código es compartible con BMPIO, también con Watcom.
  4. Se actualizan los documentos solo si cambió un formato o una decisión.
- **Specs**: los documentos de formato (`bsf_format.md`, `.bpk`) siguen existiendo porque los consume otro programa
  (BMPIO), pero se escriben junto al slice que implementa el formato, no antes.

## 10. Hoja de ruta (slices)

| # | Slice: "el usuario puede…" | Toca | Prueba manual |
|---|---|---|---|
| 0 ✅ | (base) leer/escribir `.bsf` | `deps/bsf` | tests STANDALONE (gcc + Watcom) |
| 1 ✅ | **abrir un BMP y verlo con zoom** | `platform/sdl` (SDL3), microui + `ui` (renderer, fuente, control de imagen), `core/image` (lector BMP propio: `deps/imgfile` solo lee 8 bpp y lee la cabecera como struct con `long`), `core/raster`, `deps/endian`, `hal` + `platform/sdl`, `app` | abrir `bmpio-engine/assets/j256.bmp`, hacer zoom; `--shot` para capturas sin pantalla |
| 2 | **pintar con el lápiz eligiendo color de la paleta y guardar** | `ui` (paleta), `editor` (doc, lápiz), `core/raster` o un módulo `core/draw`, `core/image` (escritura BMP) | pintar, guardar, reabrir en otro programa |
| 3 | **deshacer / rehacer** | `editor/cmd` (undo por región), atajos Ctrl+Z / Ctrl+Y | pintar varios trazos y deshacer |
| 4 | **crear un archivo nuevo** (tamaño, profundidad 1/4/8/16/24) | diálogo `ui/dlgnew`, `core/pal` (paletas por defecto) | crear 8 bpp y 1 bpp, pintar, guardar |
| 5 | **dibujar formas y ver la grilla** | línea, rectángulo, elipse, relleno, cuentagotas, grid (Ctrl+G), toolbar | dibujar cada forma con zoom y grilla |
| 6 | **convertir un BMP en secuencia y agregarle frames** | `ui/film` (film strip), `editor/doc` multi-frame, guardar/abrir `.bsf` | dibujar, "Guardar como secuencia", agregar frames, reabrir |
| 7 | **reproducir la secuencia** | `ui/play` (play/pausa, loop, delay), `hal_ticksMs` | ver la animación en loop |
| 8 | **usar onion skin** | `core/render` (composición), opciones anterior/siguiente | animar viendo el frame anterior |
| 9 | **importar una carpeta `FRAMEn.bmp` de BMPIO como secuencia** | importador en `editor`/`core/image` | convertir `bmpio-engine/assets/run/` a `.bsf` |
| 10 | **rotar los 3 colores por atajo y usar la transparencia** | `EditorState`, máscara al pegar/onion skin | — |
| 11 | **importar / exportar paletas JASC `.pal`** | `core/jascpal`, menú | ida y vuelta con otro editor |
| 12 | **pintar con pinceles, patrones y dithering** | `core/draw`, `ui` selector de pincel/patrón | — |
| 13 | **rotar la paleta y recolorear segmentos** | `core/pal`, LUTs, `core/render` | color cycling en vivo |
| 14 | **convertir 16/24 bpp a indexado** | `core/color` (cuantización, dither) | convertir una foto a 256 colores |
| — | **MVP** | | |
| 15 | usar la versión nativa de Windows | `platform/win32` | mismas pruebas de los slices 1–14 |
| 16 | agrupar secuencias en un sprite sheet `.bss` | `deps/bss`, UI de sheets; loader en BMPIO | cargar el sheet en BMPIO |
| 17 | empaquetar recursos en un `.bpk` | `deps/bpk`, herramienta de empaquetado, TEXT | leer un recurso desde BMPIO |

El orden después del slice 2 es orientativo: se puede reordenar según lo que haga falta para BMPIO.

## 11. Decisiones pendientes

1. Sintaxis del script de texto para datos de juego (por ahora `KEY=VALUE`).
2. Formato `MAP` dentro del `.bpk`.
3. Fuente bitmap: hoy se usa el atlas proporcional de la demo de microui (`ui/ui_fdata.c`, MIT). ¿Diseñar una propia estilo VGA 8×8 / 8×16?
