# Formato `.bsf` — Bitmap Sequence File (borrador v1)

Una **Secuencia** es un reel de fotogramas indexados o true-color, casi como un GIF. Sin objetos, escenas, trayectorias ni sonido.
Un BMP abierto en TookOne es una secuencia de un solo frame; guardarlo como `.bsf` no requiere conversión.

El `.bsf` es la pieza base de la familia de formatos TookOne: un **sprite sheet** agrupa varios `.bsf` y un **paquete de juego**
empotra sprite sheets (ver [`arquitectura.md`](arquitectura.md#7-formatos)). Por eso:
**una secuencia empotrada es byte a byte idéntica a un `.bsf` suelto.**

## Principios

- **Todos los frames comparten** tamaño, profundidad de color, paleta y color de transparencia (como en Take ONE: una secuencia = un formato).
- **Delay por frame** (como GIF), con un valor por defecto en la cabecera.
- **Pixeles sin pérdida**, sin compresión o con **RLE** (obligatorio de soportar; trivial en C89 y rápido en un 386).
  Deflate queda como opción futura, solo para lectores que lo soporten. Así el motor BMPIO (Watcom, DOS) no necesita zlib.
- Lectura/escritura implementada una vez en la dep genérica `deps/bsf` (`bsf.c` + `bsf.h`, C89), compartida por TookOne y BMPIO (ver `arquitectura.md` §2.3).
- **Estructura por chunks** (al estilo PNG/RIFF): un lector ignora los chunks que no conoce, así se pueden agregar cosas
  sin romper archivos viejos.
- Todo en **little-endian**.

## Estructura

```
Signature   8 bytes   "TOOK" + "BSF "        (familia + tipo de archivo)
Chunk*      repetidos hasta END
```

La firma de 8 bytes es común a toda la familia: `"TOOK"` + 4 bytes de tipo (`"BSF "`, `"SHT "`, `"PAK "`).

Cada chunk:

| Campo | Tipo | Descripción |
|---|---|---|
| `id` | 4 bytes ASCII | tipo de chunk |
| `length` | u32 | tamaño de `data` en bytes |
| `data` | `length` bytes | contenido |
| `crc32` | u32 | CRC32 de `id` + `data` |

Orden obligatorio: `HEAD`, `PLTE` (si bpp ≤ 8), `FRAM` × N, `END `. Los chunks opcionales o desconocidos pueden ir entre `HEAD` y `END `.

### `HEAD` — cabecera (obligatorio, primero)

| Campo | Tipo | Descripción |
|---|---|---|
| `version` | u16 | `1` |
| `width` | u16 | ancho en píxeles (1–4096) |
| `height` | u16 | alto en píxeles (1–4096) |
| `bpp` | u8 | 1, 4, 8, 16 o 24 |
| `flags` | u8 | bit 0: tiene color transparente · bit 1: loop |
| `transparent` | u32 | índice de paleta (bpp ≤ 8) o RGB `0x00RRGGBB` (16/24 bpp) |
| `frame_count` | u16 | número de chunks `FRAM` (≥ 1) |
| `default_delay` | u16 | ms por frame cuando `FRAM.delay` = 0 |
| `loop_count` | u16 | 0 = infinito (solo si flag loop) |

### `PLTE` — paleta (obligatorio si bpp ≤ 8)

| Campo | Tipo | Descripción |
|---|---|---|
| `count` | u16 | entradas (≤ 2^bpp) |
| `entries` | `count` × 3 bytes | R, G, B |

Equivalente directo a una paleta JASC `.pal`, así que importar/exportar paletas es trivial.

### `NAME` — nombre de la secuencia (opcional)

UTF-8 sin terminador (p.ej. `walk_left`). Lo usa el sprite sheet para indexar secuencias; en un `.bsf` suelto es informativo.

### `FRAM` — un fotograma (N veces, en orden de reproducción)

| Campo | Tipo | Descripción |
|---|---|---|
| `delay` | u16 | ms; 0 = usar `default_delay` |
| `compression` | u8 | 0 = raw · 1 = RLE · 2 = deflate (opcional, futuro) |
| `reserved` | u8 | 0 (futuro: flags por frame, p.ej. hotspot/offset) |
| `pixels` | resto | filas **de arriba hacia abajo**, sin padding entre filas, codificadas según `compression` |

RLE (estilo PackBits, por frame completo, byte a byte sobre los datos ya empaquetados):

| Byte de control `n` | Significado |
|---|---|
| 0–127 | copiar los siguientes `n + 1` bytes literalmente |
| 128–255 | repetir el siguiente byte `n − 126` veces (2–129) |

El pixel-art tiene muchas corridas del mismo índice (fondo, máscara 255), así que RLE comprime bien y se decodifica en pocas líneas de C.

Empaquetado de píxeles descomprimidos (sin padding de 4 bytes como en BMP):

| bpp | Bytes por fila | Contenido |
|---|---|---|
| 1 | ⌈w/8⌉ | índices, bit más significativo primero |
| 4 | ⌈w/2⌉ | índices, nibble alto primero |
| 8 | w | índices |
| 16 | 2·w | RGB565 |
| 24 | 3·w | R, G, B |

### `END ` — fin (obligatorio, `length` = 0)

## Reglas para el lector

1. Validar firma, que `HEAD` venga primero y la versión.
2. Ignorar los chunks desconocidos; rechazar el archivo si falta `HEAD`, `PLTE` (cuando bpp ≤ 8) o `END `.
3. Verificar que la cantidad de `FRAM` coincide con `frame_count` y que cada frame descomprime a `height × bytes_por_fila`.

## Notas para lectores en C / MS-DOS (BMPIO)

- Leer campo por campo (o con structs `#pragma pack(1)`): Watcom alinea los structs por defecto, igual que el `FileHeader` actual de BMPIO.
- Todo es little-endian, igual que x86: en DOS los enteros se leen directo.
- El CRC32 se puede verificar o ignorar (el lector del juego puede saltarlo por velocidad).
- En 8 bpp, el `transparent` del `HEAD` coincide con la convención de BMPIO de usar el índice 255 como máscara.

## Mapeo BMP ↔ BSF

| Operación | Resultado |
|---|---|
| Abrir BMP | secuencia de 1 frame, misma profundidad y paleta |
| Guardar BMP como `.bsf` | `HEAD` + `PLTE` + 1 `FRAM` |
| Carpeta `idle/FRAME1.bmp…FRAMEn.bmp` (BMPIO hoy) | importar como una secuencia de n frames |
| Agregar frame a un BMP | el documento pasa a modo secuencia; al guardar se sugiere `.bsf` |
| Guardar `.bsf` como BMP | frame actual, o todos como `nombre_000.bmp …` |
| Exportar GIF animado (futuro) | mapeo 1:1 (delay ms → centésimas, transparencia, loop) |

## Decisiones abiertas

- ¿El delay se guarda en ms (preciso) o en centésimas (compatible 1:1 con GIF)? Propuesta: ms.
- ¿Se guarda el estado del editor (frame actual, ajustes de onion skin, zoom) en un chunk opcional `EDIT`?
- Límite de tamaño: 4096 px propuesto (Take ONE limitaba a 8–2048).
- ¿Hotspot / offset por frame (útil para sprites de juego)? Se puede añadir más adelante en `FRAM.reserved` o en un chunk opcional.
