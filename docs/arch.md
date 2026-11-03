# Editor de pixel art / BMP — Redefinición y arquitectura

*Conversación del 31-10-2026 — para compartir con el proyecto en Claude Fable.*

---

## 1. Contexto (planteamiento de Vicente)

- Proyecto antiguo, abandonado: editor de imágenes BMP para pixel art y sprites (proyectos artísticos o videojuegos). Escrito originalmente en **C# / .NET / WinForms**.
- Nueva intención: **portabilidad**. Separar en capas:
  - una capa de compatibilidad con el sistema operativo, fácil de reimplementar (Linux, Mac, etc.);
  - un núcleo funcional independiente.
- Alternativas consideradas: C puro + GTK; Python + Qt (descartado: no quiere Python para esto); mantener C# + WinForms (descartado: lo percibe como muy atado a Microsoft).
- Referencia funcional: **Take One**, editor de pixel art para Windows 16/32 bits (época Win98/XP) que usaba mucho. Problemas: funciones desactualizadas, incompatibilidades, y controles de UI propietarios y rígidos (p. ej. la paleta de colores no se puede mover, acoplar ni redimensionar).
- Plan paralelo: ingeniería inversa de Take One con Claude para entender manejo de píxeles, transformaciones y colores a bajo nivel.
- Antecedente: hace un par de semanas el plan era C puro + Win32, apuntando a Windows 32 bits en adelante, con área de trabajo propia (cuadrícula, reglas, zoom).

Pregunta: opinión, ideas y referencias de arquitectura para software de escritorio.

---

## 2. Opinión general

**Las dos ideas (C + Win32 y "portable por capas") no compiten: Win32 puede ser simplemente el primer backend.** Si el núcleo se diseña bien desde el día uno, empezar por Win32 no cierra ninguna puerta.

Una corrección sobre C#: .NET hoy es open source (licencia MIT, .NET Foundation) y corre en Linux y Mac. Lo que sí es exclusivo de Windows es **WinForms**. Es decir, C# no es el problema; WinForms sí. Si algún día se quisiera C#, existen Avalonia o Eto.Forms. Dicho eso, dado tu perfil (C, bajo nivel, DOS, HAL de video) creo que **C es la elección correcta** para este proyecto, por afinidad y por control, no por ideología.

La decisión realmente importante no es el lenguaje, sino **cómo se dibuja la interfaz**. Eso define cuánto mide la "capa del SO".

---

## 3. La pregunta que define todo: ¿UI nativa o UI propia?

| Enfoque | Qué es la capa de plataforma | Pros | Contras |
|---|---|---|---|
| **A. UI nativa por SO** (Win32 en Windows, GTK en Linux, Cocoa en Mac) | Enorme: ventanas, menús, diálogos, paneles, controles… todo por SO | Look nativo, accesibilidad, integración total | Reescribes la UI entera por cada plataforma. Acoplar/mover paneles en Win32 puro es mucho trabajo |
| **B. Un toolkit portable** (GTK en todas partes, o Qt) | La pone el toolkit | Un solo código de UI. Qt trae docking resuelto (`QDockWidget`) | GTK en Windows/Mac se ve y empaqueta regular. Qt implica C++ |
| **C. UI propia dibujada sobre una capa mínima** (SDL3, o tu propio backend Win32/X11) | Pequeña: ventana, eventos de input, blit de framebuffer, diálogos de archivo, portapapeles | Capa de SO mínima y fácil de portar. Control total de cada píxel de la UI. Paneles flotantes/acoplables a tu manera | Hay que construir los widgets (o usar una librería immediate-mode). Sin look nativo |

**Mi recomendación: opción C.** Es exactamente lo que hacen los editores de pixel art serios:
- **Aseprite**: UI propia sobre su librería `laf`, que tiene una capa `os` con backends Win32 / macOS / X11.
- **GrafX2**: C puro, SDL, UI propia, heredero de Deluxe Paint, portado a muchísimas plataformas.

En pixel art el look nativo importa poco; lo que importa es precisión, respuesta y un layout flexible. Además, la opción C encaja con lo que ya hiciste en el juego de DOS: el HAL de video con page flipping es conceptualmente la misma capa de plataforma.

Matiz honesto: si quieres que corra en **Windows 98/XP**, verifica qué versiones de SDL siguen soportándolos (SDL3 no). En ese caso, tu propio backend Win32 (GDI + `StretchDIBits`) como capa de plataforma es lo más seguro, y SDL quedaría como backend para Linux/Mac.

---

## 4. Arquitectura propuesta (capas)

Regla de oro: **las dependencias apuntan hacia adentro**. El núcleo nunca incluye `windows.h`, `gtk.h` ni `SDL.h`.

```
┌──────────────────────────────────────────────┐
│  platform/  win32/  sdl/  (gtk/ cocoa/ ...)  │  ventana, input, blit, diálogos, portapapeles, timers
├──────────────────────────────────────────────┤
│  ui/        widgets propios, layout, paneles │  dibuja sobre un framebuffer, sin saber del SO
├──────────────────────────────────────────────┤
│  editor/    documento, herramientas, comandos│  estado de la app, undo/redo, viewport (zoom/pan)
├──────────────────────────────────────────────┤
│  core/      imagen, paleta, primitivas, I/O  │  C99, solo libc, testeable sin ventana
└──────────────────────────────────────────────┘
```

### core/ (biblioteca pura)
- `image`: buffer de píxeles; idealmente **modo indexado (8 bpp + paleta de 256)** como modo principal, que es lo natural en pixel art (swaps de paleta, rampas, color cycling estilo Deluxe Paint), y RGBA como opción.
- `palette`: gestión de paleta, rampas, ordenamiento, import/export (GPL, PAL, ACT…).
- `draw`: Bresenham para líneas, punto medio para elipses, rectángulos, **flood fill por scanlines** (no recursivo), pinceles.
- `transform`: flip, rotaciones de 90°, escalado nearest-neighbor, desplazamiento con wrap.
- `selection`: máscara de selección, copiar/pegar como imagen flotante.
- `bmp_io`: lector/escritor BMP propio (1/4/8/24/32 bpp). Es un formato simple; escribirlo tú mismo es parte de la gracia. PNG después vía `stb_image`/`lodepng` si quieres.

### editor/ (portable, sin SO)
- `document`: imagen + paleta + metadatos + estado sucio.
- `tools`: cada herramienta como máquina de estados que recibe **eventos abstractos** (`press/move/release` en coordenadas de imagen).
- `commands` + `undo`: patrón Command. Para pixel art, un undo por **diff de región** (bounding box del trazo, guardando antes/después) es simple y eficiente. Aseprite y Krita usan undo por tiles si algún día escalas.
- `viewport`: matemática pura de zoom, pan, conversión pantalla↔imagen, cuadrícula y reglas.

### ui/
- Widgets propios que se dibujan en un framebuffer: barra de herramientas, **paleta como panel flotante/acoplable/redimensionable** (justamente lo que Take One no permite), selector de color, capas (si llegas a eso).
- Para no escribir todo desde cero: **Nuklear** o **microui** (C, single-header, immediate-mode). Dear ImGui (rama docking) resuelve el acople de paneles pero es C++ (existe `cimgui` como binding C).

### platform/ (interfaz chica y explícita)
```c
/* platform.h — lo único que cada backend debe implementar */
int   plat_init(const plat_config *cfg);
int   plat_poll_event(app_event *ev);          /* traduce WM_*, SDL_Event, GdkEvent → app_event */
void  plat_present(const uint32_t *fb, int w, int h);  /* blit del framebuffer */
int   plat_open_file_dialog(char *out, int max, const char *filter);
int   plat_save_file_dialog(char *out, int max, const char *filter);
int   plat_clipboard_set_image(const image *img);
uint64_t plat_ticks_ms(void);
void  plat_shutdown(void);
```
El backend Win32 sería: `RegisterClass` + `WndProc` que llena `app_event`, y `StretchDIBits` para presentar. El de SDL: `SDL_PollEvent` + una textura streaming. Mismo contrato.

Patrón de referencia directo: **Handmade Hero** (Casey Muratori). La plataforma es dueña del loop y llama a la app; la app expone `update/render` y recibe servicios. Muy en tu línea.

### Beneficio inmediato
`core/` y `editor/` se pueden probar con tests unitarios por consola (dibujar una línea, hacer fill, deshacer, comparar con un BMP esperado) sin abrir ninguna ventana.

---

## 5. Sobre la ingeniería inversa de Take One

- **Empieza por análisis de comportamiento, no de binario.** Documenta cómo se comporta cada herramienta: atajos, cómo pinta el lápiz al mover rápido (¿interpola líneas entre eventos?), cómo funciona el fill (¿4 u 8 vecinos?), qué pasa con la paleta al cambiar un color, comportamiento con zoom, etc. Ese documento se convierte en tu **especificación funcional**, que es lo que realmente necesitas.
- Si algún algoritmo no queda claro observándolo, ahí sí desensamblar con **Ghidra** (soporta ejecutables NE de 16 bits y PE de 32 bits).
- Ojo legal: estudiar el comportamiento y reimplementarlo con tu propio código está bien; copiar código o recursos (iconos, gráficos) no. Lo sano es un enfoque tipo *clean room*: entender → especificar → implementar desde la spec. Revisa la licencia/EULA del programa.
- La mayoría de los algoritmos de bajo nivel (líneas, fill, rotaciones, cuantización) son estándar y están documentados públicamente; probablemente Take One no haga nada exótico ahí. Donde sí vale la pena mirar es en **decisiones de UX**: qué hacía cómodo dibujar con él.

---

## 6. Referencias

**Código abierto para estudiar (muy recomendados):**
- **GrafX2** — C, SDL, UI propia, inspirado en Deluxe Paint. La referencia más cercana a lo que quieres.
- **mtPaint** — C + GTK, editor de pixel art y paletas indexadas. Ejemplo del enfoque "C + toolkit".
- **Aseprite** (código disponible) y **LibreSprite** (fork GPL) — mira especialmente la librería `laf` y su capa `os`.
- **Pixelorama** — hecho sobre Godot; útil para ver features modernas de pixel art.
- **GIMP** — ejemplo de núcleo separado como librerías (GEGL para procesamiento, babl para formatos de píxel).

**Patrones y lectura:**
- *Ports and Adapters / Arquitectura hexagonal* (Alistair Cockburn) — la idea formal detrás de "núcleo + adaptadores de SO".
- *Game Programming Patterns* (Robert Nystrom, gratis online) — capítulos de Command (undo), Game Loop, Double Buffer.
- *Handmade Hero* — capa de plataforma en C, desde cero.
- MVC / MVP — para separar documento, vista y controlador; en editores gráficos suele terminar siendo Documento + Vistas + Herramientas/Comandos.
- *The Architecture of Open Source Applications* (gratis online) — estudios de caso de arquitecturas reales.

**Librerías útiles en C:**
- SDL3 (o SDL2 si necesitas plataformas más antiguas) — backend de plataforma.
- Nuklear / microui — UI immediate-mode en C.
- stb_image / stb_image_write — formatos adicionales.
- tinyfiledialogs — diálogos de archivo nativos portables, si no quieres implementarlos por backend.

---

## 7. Siguientes pasos sugeridos

1. Escribir la **spec funcional** a partir de Take One (herramientas, atajos, paleta, comportamiento de zoom) y decidir el alcance de la v1.
2. Definir el modelo de datos del core (¿solo indexado? ¿capas en v1? probablemente no).
3. Implementar `core/` + `bmp_io` con tests por consola.
4. Definir `platform.h` y hacer el primer backend (Win32 o SDL, según si Win98/XP sigue siendo objetivo).
5. Canvas con zoom, cuadrícula y reglas → lápiz → undo. Recién entonces el resto de herramientas y la UI de paneles.

**Pregunta abierta que conviene cerrar antes de escribir código:** ¿Windows 98/XP es un objetivo real o un "sería lindo"? Eso decide si la capa base es Win32 propio o SDL.

---

## 8. Decisión: opción B (toolkit portable)

Vicente elige la **opción B**: construir widgets propios es demasiado trabajo.

**B son dos caminos:**
- **GTK4 (C)**: sigue en C puro. Contras: GTK4 no trae docking de paneles de serie (solo `GtkPaned`; docking real vía `libpanel`, orientada a GNOME y poco probada fuera de Linux). En Windows funciona, pero el empaquetado es tedioso (MSYS2 + muchas DLLs) y el look es algo ajeno.
- **Qt (C++)**: `QDockWidget` resuelve paneles flotantes/acoplables/redimensionables (justo la crítica a Take One). Buen look y empaquetado en Windows/Linux/Mac. Costo: C++, aunque se puede usar un subconjunto sobrio y llamar al núcleo en C sin problemas.

**Criterio:** si la flexibilidad de paneles es prioritaria → Qt. Si C puro es innegociable → GTK, aceptando paneles más rígidos en la v1.

**Qué no cambia:**
- El **canvas es propio** en cualquier caso: `GtkDrawingArea` o `QWidget::paintEvent`, pintando el framebuffer escalado nearest-neighbor + cuadrícula + reglas.
- `core/` y `editor/` siguen en C sin dependencias. La capa `platform/` se reemplaza por una capa **frontend** que traduce eventos del toolkit a eventos de herramientas; cambiar de toolkit = reescribir solo esa capa.
- **Windows 98/XP queda descartado** (ni GTK4 ni Qt6 corren ahí), lo que cierra la pregunta abierta de la sección 7.

---

## 9. Aclaraciones y decisión final: Qt + C++ (núcleo en C)

**Aclaraciones previas a la decisión:**
- Qt **no es Python**: es un framework C++ que compila a nativo. PyQt/PySide son solo bindings.
- Elegir GTK/Qt **no sacrifica rendimiento** frente a Win32: en un editor de pixel art el cuello de botella es el redibujado del canvas (tu código), no el toolkit. GTK4 incluso puede rendir peor que Qt Widgets en equipos viejos por depender de GPU.
- Sensación nativa por opción: Win32 → nativo solo en Windows (Linux vía Wine; se puede compilar desde Fedora con MinGW). GTK → nativo en GNOME, ajeno en Windows y algo en KDE. Qt → estilo nativo en Windows, nativo en KDE; portapapeles, diálogos, drag & drop y atajos se comportan como espera el usuario en cada SO.
- Criterios de Vicente (sensación nativa, portabilidad, rendimiento, desarrollo simultáneo en Windows y Linux) → Qt los cumple todos; el único costo es C++ en la capa de interfaz.

**Decisión: Qt con C++ para la interfaz, núcleo en C puro.**

**Lineamientos técnicos:**
- **Qt Widgets, no QML/Qt Quick**: Widgets es lo nativo de escritorio, liviano y con `QDockWidget` para la paleta y demás paneles; QML depende de GPU y apunta a otro tipo de apps.
- **Qt5 vs Qt6**: Qt6 exige Windows 10+ de 64 bits; Qt 5.15 llega a Windows 7. Elegir según el piso de compatibilidad deseado (por defecto: Qt6).
- **Build**: CMake (estándar en Qt6, funciona igual en Windows y Fedora).
- **Licencia**: Qt bajo LGPL permite distribuir enlazando dinámicamente, sea el proyecto libre o no.
- **Núcleo en C**: `core/` y `editor/` como librería C estática; headers con `extern "C"` para consumirlos desde C++. El núcleo no conoce Qt.
- **Canvas**: `QWidget` propio con `paintEvent`. El framebuffer del núcleo se envuelve en un `QImage` sin copiar (`Format_Indexed8` + tabla de colores, o `Format_ARGB32`), se dibuja escalado con `QPainter` sin suavizado (`SmoothPixmapTransform` desactivado) y se redibuja solo la región modificada (`update(QRect)`).
- **Frontend**: traduce `QMouseEvent`/`QTabletEvent`/`QKeyEvent` a eventos abstractos de herramientas; portapapeles vía `QClipboard`, diálogos vía `QFileDialog`.
- **C++ sobrio**: clases simples para la UI, sin metaprogramación; la lógica vive en C.

**Enfoque mixto (Vicente no es experto en C++): C++ solo como "pegamento" entre Qt y el núcleo.**
- Expectativa realista: la capa C++ (ventana, canvas, docks, menús, señales) será ~20–30 % del código, pero de un tipo acotado y repetitivo.
- C++ necesario: heredar de `QWidget` y sobrescribir `paintEvent`/`mouse*Event`; `connect()` con lambdas; modelo padre-hijo de Qt (casi nunca `delete`); `QString`/`QImage`. Se puede ignorar: plantillas propias, herencia compleja, excepciones, casi toda la STL.
- Regla: **si se puede expresar sin Qt, va en C** (lógica de herramientas, undo, zoom/pan y conversión de coordenadas, operaciones de paleta). El widget solo recibe eventos, llama al núcleo y pinta.
- Frontera C/C++: **handles opacos** (`doc_t*` + funciones `doc_*()`), y el núcleo **devuelve rectángulos sucios** (p. ej. `tool_mouse_move()` retorna la región cambiada → `update(rect)`) en lugar de usar callbacks entre lenguajes.

**Siguientes pasos:**
1. Esqueleto CMake: `core/` (C, con tests por consola) + `app/` (Qt Widgets).
2. `core`: imagen indexada + paleta + `bmp_io`.
3. Ventana principal con `QMainWindow`, canvas central con zoom/cuadrícula, paleta en `QDockWidget`.
4. Lápiz + undo → resto de herramientas.
5. En paralelo: spec funcional a partir del análisis de comportamiento de Take One.
