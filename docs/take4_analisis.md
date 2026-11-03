# Análisis de Take ONE 4 (ingeniería inversa parcial)

Análisis estático de `docs/take4/` (sin ejecutar el binario): cabeceras PE, tablas de import/export,
recursos (menús, diálogos, string tables, aceleradores, bitmaps) y desensamblado puntual.
Se contrasta contra [`base_reqs.md`](base_reqs.md).

> Leyenda de confianza: **[C]** confirmado en el binario · **[I]** inferido (alta probabilidad) · **[H]** hipótesis a validar con archivos de ejemplo.

---

## 1. Identidad del binario

| Campo | Valor |
|---|---|
| Producto | Take ONE 4.0.000 (`TAKEONE.EXE`) |
| Autor | Rainer Döbele — DocSystems, Tuttlingen (DE), © 1992-2000 |
| Plataforma | PE32 i386, Win32 GUI, GDI puro (sin DirectX), runtime MSVC |
| Imports del EXE | KERNEL32, USER32, GDI32, comdlg32, ADVAPI32, SHELL32, WINMM |
| Edición | Shareware con licencia (diálogo *Registration*, textos "full version only") |
| Idioma | Inglés con restos en alemán (`Zusatzoptionen`, `Bildsequenz`, `Strg+F7`) |

## 2. Arquitectura

```
TakeONE4.exe  ── núcleo: editor, film strip, paleta, animación, zlib embebido
 ├─ TakeConv32.dll ── plugin de conversión de formatos (usa Ijl11.dll para JPEG)
 ├─ TakeExp32.dll  ── exportador ANS → AVI (usa TakeVfw32.dll)
 │   └─ TakeVfw32.dll ── wrapper Video for Windows (MSVFW32 / AVIFIL32)
 └─ Ijl11.dll      ── Intel JPEG Library 1.1 (third-party)
```

### 2.1 API del plugin de formatos (TakeConv32.dll) **[C]**

Exports: `GetFormats`, `DetectFileFormat`, `GetPreviewImage`, `LoadConvImage`, `SaveConvImage`,
`NewConvFile`, `GetDesiredBitPix`, `NotifyFrameChange`, `ProcessCommand`.

Es exactamente el patrón *codec registry* que conviene para TookOne:

| Export | Rol | Equivalente moderno sugerido |
|---|---|---|
| `GetFormats` | lista filtros/extensiones soportadas | `IImageCodec.Formats` |
| `DetectFileFormat` | sniffing por firma | `bool CanRead(Stream)` |
| `GetPreviewImage` | miniatura para el diálogo Abrir | `Thumbnail(Stream)` |
| `LoadConvImage` / `SaveConvImage` | decodificar / codificar | `Read` / `Write` |
| `GetDesiredBitPix` | profundidad de color que exige el formato | `SupportedDepths` |
| `NewConvFile`, `NotifyFrameChange`, `ProcessCommand` | hooks del plugin hacia el editor (añade menú *Animation…*, *Change Format…*) | eventos/comandos del plugin |

### 2.2 Controles de ventana propios **[C]**

`FILMSTRIP` (tira de frames), `PRINTFILM`, `AVIREEL`, `REGION`, `TABCTL`, `STATICLINE`, `ANIEDIT`,
`TakeONE_ToolBar`, `TakeONE_ToolTip`. El *film strip* es el componente central de la UI.

## 3. Modelo de datos y formatos

### 3.1 Conceptos

- **Frame**: un bitmap DIB (1/4/8/16/24 bpp). Tamaño permitido **8–2048 px** por lado **[C]** (string 2017).
- **Frame Sequence** (`.BMX`, "Bildsequenz"): lista de frames del mismo formato + paleta + color transparente. ← **equivale a la Secuencia `*.bsf` de TookOne**.
- **Animation** (`.ANS`): escena que contiene **Objects** (`.ANO`), cada uno apunta a una secuencia y tiene animación por *secuencia*, *ciclo* (con condiciones/branches/contadores/teclas/mouse) o *path*; más regiones de primer plano, offsets por frame, sonidos WAV/MIDI. Se reproduce con `RUNTAKE.EXE` o como salvapantallas.
- **ANX**: paquete exportable de un `.ANS` + todos sus recursos (`anxinfo.dat`).

### 3.2 Formatos soportados

| Formato | Lectura | Escritura | Dónde |
|---|---|---|---|
| BMP / DIB / RLE | ✓ | ✓ | núcleo |
| BMX (secuencia propia, comprimida) | ✓ | ✓ | núcleo |
| ANS / ANO / ANX | ✓ | ✓ | núcleo |
| GIF / GIF animado | ✓ (opciones si los frames tienen tamaños distintos) | ✓ (loop, delay 1/100 s, disposal, interlaced, transparencia) | TakeConv32 |
| PCX, TIF/TIFF | ✓ | ✓ | TakeConv32 |
| ICO / CUR / ANI | ✓ | ✓ (editor de cursor animado, hotspot, 16/32/64 px) | TakeConv32 |
| JPEG | ✓ | ✓ | TakeConv32 + Ijl11 |
| Bitmaps de recursos EXE/DLL/SCR | ✓ | — | TakeConv32 |
| AVI | ✓ (selección de frames, paleta, dither) | ✓ (codec VfW, audio) | TakeConv32 / TakeExp32 |
| Secuencia de imágenes sueltas (`Frame%d.bmp`) | ✓ | ✓ | núcleo |
| **JASC .PAL** | ✗ | ✗ | — (no existe en Take ONE) |

### 3.3 Cabeceras (desensamblado)

- **BMX [C/H]**: se leen 14 bytes; `WORD` en offset 0 = `0x5842` → ASCII **`"BX"`** (análogo a `BITMAPFILEHEADER` con `"BM"`). El `DWORD` en offset 2 dimensiona un buffer (`GlobalAlloc(size+4)`), luego se lee un `BITMAPINFOHEADER` (0x28 bytes). Error `0x8004` = "Unknown file format". **[C]**
  Compresión: el EXE contiene zlib (`oversubscribed … tree`, `incompatible version`) y el ini tiene `BMX-Compression=546` → los frames BMX probablemente van deflate-comprimidos **[I]**.
- **ANS [C/H]**: se leen 11 bytes; `WORD` en offset 0 = `0x4E41` → **`"AN"`**. Resto sin decodificar **[H]**. Error 28676 "created for a newer version" ⇒ hay campo de versión.
- Para cerrar el formato hace falta un `.bmx` / `.ans` de muestra (no hay en `docs/take4`).

## 4. Inventario de funcionalidades (UI)

### 4.1 Herramientas (toolbar, iconos 101-166)

Lápiz libre · Línea · Rectángulo (Ctrl = cuadrado) · Elipse (Ctrl = círculo) · Arco · Polígono · Relleno ·
Spray · Texto (fuente, tamaño, B/I/U, alineación) · Cuentagotas · Selección rectangular · Selección poligonal ·
Zoom ×1/×2/×4/×8 · Frame anterior/siguiente · Grid · Onion skin · Zoom de film strip.

Selector de **5 tamaños de pincel** (bitmap 920) y **12 patrones de relleno editables** guardables (diálogo *Edit Fill Patterns*).
Barra de estado contextual (`Draw Line from (%d/%d) Length %d`, `Pen Color: R G B Index`, etc.).

### 4.2 Transformaciones de selección (bitmap 909)

Stretch, sheer H/V, rombo H/V, trapecio H/V, elipse H/V; scroll con wrap (Ctrl+flechas); flip H/V;
rotación ±90° (Alt+R / Alt+L) y libre en grados con **9 centros de rotación** y "delete vacant pixels".

### 4.3 Color y paleta

- *New Frame Sequence*: tipo, nombre, ancho/alto, **2 / 16 / 256 / 65536 / 16,7M colores**, color transparente (None / clipboard).
- *Change Frame Format*: redimensionar (aspect ratio, stretch, usar selección) + cambiar profundidad con **Dithering**, **Use Frame Colors** o **Quantize Colors**.
- *Color Palette Request* al abrir/pegar con paleta distinta: usar paleta estándar · dither a la paleta · cargar paleta original (configurable en *Editor Settings*).
- *Edit Palette* (RGB por entrada), *Reset Palette* (Ctrl+Alt+R), *Detect used colors* (Ctrl+Alt+U), paleta flotante (Ctrl+F6).
- *Replace Color*: origen RGB ± tolerancia, destino en **HSL**, "Keep Color Differences" (re-colorea un rango manteniendo matices), preview.
- *Adjust Colors*: brillo, contraste, tono, saturación, R, G, B (alterar % o fijar).
- **Pen color + Fill color** (dos colores activos) + **color transparente por secuencia** (16 swatches + None, tolerancia ±%, "Build mask at runtime", reemplazar en todos los frames).
- *Gradated Lines* (F5): líneas con gradación de color.

### 4.4 Efectos

Blur / Sharpen (soft/medium/hard), Stamp (emboss gris/color), Shake (H/V/ambos), Greyscale, Invert y
**convolución 3×3 personalizada** (coeficientes −9…+9, rango 0-255 o ±127).

### 4.5 Secuencia / film strip

Film strip a la **derecha (F6)** o **abajo (F7)**, zoom de miniaturas (Ctrl+F7), `TrackFilm`.
Insert/Add frame vacío o copia, Delete, Swap (Alt+S), Move (Alt+M), Replace frame from file,
Paste into sequence, **Select All Frames (Tab)** ⇒ edición simultánea de varios frames, offsets X/Y por frame.

### 4.6 Onion skinning **[C]**

Toggle **Ctrl+F5**; opciones *Previous Frame*, *Next Frame*, *Any Frame*. Clave ini `GreaseProofMode`
("papel de calco"). Usa el color transparente de la secuencia para el enmascarado.

### 4.7 Otros

Undo/Redo con buffer en KB (`UndoBufferSize`, `SoftUndo`), MRU, impresión con layout por drag & drop,
grid (Ctrl+G) con centro de elipse en centro/esquina de celda, "Emulate Fill", arrastrar selección con botón izq./der.

### 4.8 Atajos principales **[C]** (tabla ACCEL 950)

| Acción | Atajo | Acción | Atajo |
|---|---|---|---|
| Nuevo / Abrir / Guardar | Ctrl+N / Ctrl+O / Ctrl+S | Undo / Redo | Ctrl+Z, Alt+Bksp / Ctrl+Y |
| Añadir frames | Ctrl+A | Guardar frame | Ctrl+U |
| Limpiar selección/frame | Space / Ctrl+E | Seleccionar todos los frames | Tab |
| Onion skin | Ctrl+F5 | Grid | Ctrl+G |
| Gradated lines | F5 | Film strip der./abajo | F6 / F7 |
| Paleta flotante | Ctrl+F6 | Zoom film strip | Ctrl+F7 |
| Insertar frame vacío / copia | Alt+Ins / Ctrl+Alt+Ins | Añadir vacío / copia | Alt+Num+ / Ctrl+Alt+End |
| Borrar frame | Alt+Del, Alt+Num− | Primer / último frame | Home / End |
| Flip H / V | Alt+H / Alt+V | Rotar 90° der./izq. | Alt+R / Alt+L |
| Rotar libre | Ctrl+R | Reemplazar color | Alt+C |
| Scroll selección | Ctrl/Shift+flechas | Cambiar formato | Ctrl+F |

### 4.9 Configuración (`TakeONE4.ini`)

Secciones `[Window]`, `[File]`, `[Edit]`, `[Print]`, `[MRUFiles]`. Claves relevantes:
`UseSystemPalette`, `UseClipboardPalette`, `BMX-Compression`, `UseSavedPalette`, `AskStretchFrame`,
`DitherOnOpen`, `DitherOnPaste`, `UndoBufferSize`, `ViewSize%d`, `GreaseProofMode`, `TrackFilm`, `EditBorder`.

## 5. Contraste con `base_reqs.md`

| Requisito TookOne | Take ONE 4 | Estado |
|---|---|---|
| Profundidades 1/4/8/16/24 bits | Idénticas (2/16/256/65K/16,7M) | **Paridad directa** |
| Importar/exportar paletas **JASC .pal** | No existe | **Nuevo** |
| Convertir 16/24 bit → indexado según paleta cargada | *Change Format* + *Palette Request* (dither / quantize / frame colors) | Paridad; replicar el flujo de 3 opciones |
| Rotación de paleta (color cycling) | No existe | **Nuevo** (estilo DPaint) |
| Lookup tables / recolor de segmentos | Parcial: *Replace Color* HSL + "Keep Color Differences", *Adjust Colors* | **Extender** a rangos de paleta |
| Pinceles de tamaño/color/patrón | 5 tamaños + 12 patrones editables | Paridad, ampliar a pinceles custom (brush = selección) |
| Gradientes y dithering en pincel (DPaint) | Solo *Gradated Lines* + dither en conversión | **Nuevo** |
| 3 colores: primario, secundario, transparente | Pen + Fill + transparente por secuencia (con tolerancia) | Paridad conceptual; **nuevo**: rotación por atajo |
| Masking por color transparente al copiar entre bitmaps | "Build Mask at Runtime", pegado con transparencia | Paridad |
| Rejilla de píxel | Ctrl+G | Paridad |
| Visor de frames con miniaturas | Film strip (der./abajo, zoom, multi-selección) | Paridad |
| Onion skinning anterior/siguiente | Previous / Next / **Any** frame | Paridad (+ opción "Any") |
| Secuencia `*.bsf` | En Take ONE es **`.BMX`** (secuencia); `.ANS` es otra cosa (escena con scripting) | Resuelto: formato propio `.bsf` |

## 6. Recomendaciones para TookOne

1. **Nomenclatura** — *decidido*: una animación se llama **Secuencia** (como en Take ONE) y se guarda en `.bsf` (*Bitmap Sequence File*), un formato propio y mínimo (reel de frames, tipo GIF), sin objetos ni trayectorias. BMP = secuencia de 1 frame, promovible al guardar. Ver [`bsf_format.md`](bsf_format.md) y `base_reqs.md`. Sin compatibilidad binaria con `.ANS`.
2. **Fuera de alcance del MVP** (existen en Take ONE pero no aportan al objetivo pixel-art): objetos/escenas `.ANS`, scripting de ciclos/condiciones/paths, regiones, sonido, runtime/salvapantallas, ANX, impresión, instalación en Program Manager, DDE, registro.
3. **Adoptar del original**: arquitectura de **codecs como plugins** (sección 2.1), film strip como control propio, flujo *Palette Request* al abrir/pegar, transparencia con tolerancia, multi-selección de frames para editar en lote, undo con límite en memoria, barra de estado contextual por herramienta, transformaciones de selección (sheer/rombo/trapecio).
4. **Import opcional de legado**: un codec lector de `.BMX` sería valioso para migrar trabajos antiguos; requiere un archivo de muestra para completar la sección 3.3.
5. **Formatos prioritarios** para el MVP: BMP (1/4/8/16/24), PNG indexado, GIF animado, JASC PAL; luego PCX/ICO/CUR/ANI.

## 7. Pendientes

- [ ] Conseguir `.bmx` y `.ans` de ejemplo para decodificar los formatos (sección 3.3).
- [ ] Capturas del original en ejecución (Wine) para documentar el layout de la ventana principal.
- [ ] Validar valor de `BMX-Compression=546` (¿flags de compresión / nivel?).
