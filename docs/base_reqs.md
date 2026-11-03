# TOOK ONE 

TookOne es un editor de bitmaps enfocado en trabajos pixel-art, animaciones y sprites, fuertemente influenciado en un editor que usaba tiempo atrás "Take One 4", el cual debido a su antiguedad se ha quedado obsoleto en muchas funcionalidades y
flexibilidad, por ende, he decidido escribir una copia modernizada de este editor.

Este programa ofrece una interfaz sencilla e intuitiva, tomada directamente de TakeOne 4, ofrece las siguientes funcionalidades el cual se representan en dos modos ya que el editor gestiona archivos BMP o secuencias (*.bsf)

### Editor de bitmaps

- Paletas ajustadas: Este editor en su MVP soporta manejar sprites y bitmaps de color depth de 1, 4, 8, 16 y 24-bits
customizables donde el usuario podra importar o exportar desde archivos JASC *.pal y conversion de imagenes de 
16, 24 bits a color indexado segun las paletas cargadas.
- Rotacion de paletas, lookup tables y recolor de segmentos de paletas
- Pinceles con gradientes y dithering: Pinceles de tamaño, color o patron seleccionable, dithering con patron seleccionable como en "Deluxe Paint" para distintos efectos practicos de intensidad o blending de colores. 
- El usuario podra seleccionar 3 colores simultaneos, el primario, secundario y terciario (de transparencia), rotables por shortcut de teclado.
    - El color de transparencia es usado para hacer masking de transparencia cuando hay match, util para copiar un bitmap de un origen a otro y ajustar
    la transparencia automaticamente, o para el onion skinning (Ver mas abajo.)
- Rejilla: El usuario podra activar una rejilla que permite visualizar cada pixel en el canvas.

### Editor de secuencias

Terminologia (igual que en Take ONE 4): una animacion se llama **Secuencia** y se guarda como `*.bsf` (*Bitmap Sequence File*).

Cuando el usuario va manipular una secuencia (*.bsf)
- Visor de frames: El usuario tendra acceso a un visor de los frames dentro del workspace, a modo de una lista con miniaturas sucesivas y seleccionables
para editar el frame puntual.
- Controles de reproduccion: play/pausa, frame anterior/siguiente, primero/ultimo, loop on/off y velocidad (delay por frame), para previsualizar la animacion dentro del editor.
- Onion Skining: El usuario tendra acceso a una funcionalida que permite visualizar el frame siguiente o el anterior dentro del mismo frame actual, a modo de
transparencia o segun el color de transparencia seleccionado. 

### Modelo de documento: un solo tipo, dos modos

El formato `*.bsf` es deliberadamente simple, casi como un GIF: **un reel de fotogramas y nada mas**. No hay objetos, escenas,
trayectorias, condiciones ni sonido (a diferencia de los `.ANS` de Take ONE 4). Especificacion en [`bsf_format.md`](bsf_format.md).

- Internamente existe un unico documento, la **Secuencia**: paleta + color de transparencia + lista de frames (todos del mismo tamaño y profundidad).
- **Un BMP es una secuencia de un solo frame.** El modo "Editor de bitmaps" es simplemente esa secuencia sin el panel de frames.
- **Promocion BMP → secuencia**: editando un BMP, el usuario puede "Guardar como secuencia" (`*.bsf`); el documento pasa a modo secuencia
  y desde ahi agrega frames. Esto agiliza el testeo al diseñar sprites (dibujar una pose, convertir, iterar).
- **Secuencia → BMP**: guardar como BMP una secuencia con varios frames exporta el frame actual o todos los frames como BMPs numerados.
- Agregar un frame a un BMP abierto tambien lo promueve a secuencia (se pedira guardar como `*.bsf` al guardar).

### Futuro: Sprite sheets y paquetes de juego

- **Sprite sheet**: un archivo que agrupa varias secuencias con nombre (p.ej. `idle`, `walk`, `jump` de un personaje u objeto).
- **Paquete de juego** (estilo WAD de Doom): un contenedor con sprite sheets empotrados junto a otros recursos (stats de personajes, items, mapas, texto, etc.).
- Ambos se diseñan desde ya como contenedores del `.bsf`: una secuencia empotrada es byte a byte igual al archivo `.bsf` suelto. Ver [`arquitectura.md`](arquitectura.md#7-formatos).


## Pasos

* [x] Crear una nueva rama con las redefiniciones del proyecto (`feat/redefinition`)
* [ ] Definir un plan de arquitectura escalable y portable para desarrollar software de escritorio (borrador en [`arquitectura.md`](arquitectura.md); stack: C89 + microui + SDL)
* [x] Hacer un analisis de Take One 4 con IA y hacer una ingenieria inversa parcial. Ver [`take4_analisis.md`](take4_analisis.md).
* [ ] Definir el formato `*.bsf` (borrador en [`bsf_format.md`](bsf_format.md))
* [ ] Reconstruir el proyecto desde cero en C89 (microui + SDL) por vertical slices (hoja de ruta en [`arquitectura.md`](arquitectura.md#10-hoja-de-ruta-slices))

