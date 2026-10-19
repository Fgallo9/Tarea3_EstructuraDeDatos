# GraphQuest - Laberinto en un Grafo

## Descripción

Esta aplicación en C implementa un juego de exploración llamado GraphQuest. El laberinto se carga desde un archivo CSV y se representa en memoria como un grafo, donde cada nodo es un escenario con una descripción, ítems coleccionables y conexiones en cuatro direcciones (arriba, abajo, izquierda y derecha). Utilizando Tipos de Datos Abstractos (TDAs) como Grafos y Listas, el juego permite recorrer el laberinto, recoger y descartar ítems, y llegar a la salida con el mayor puntaje posible antes de que se acabe el tiempo.

Reglas principales:
- El tiempo inicial es 10.
- Recoger ítems o descartar ítems cuesta 1 de tiempo (sin importar cuántos se elijan a la vez).
- Avanzar cuesta `techo((peso total del inventario + 1) / 10)` de tiempo.
- Se gana al llegar al escenario final con tiempo disponible; se pierde si el tiempo se agota antes.
- El puntaje es la suma de los valores de los ítems que hay en el inventario.

## Cómo compilar y ejecutar el programa

1. **Descarga y descomprime** el archivo `.zip` del proyecto en una carpeta de tu preferencia. La carpeta debe contener `tarea3.c`, `lista.c`, `lista.h`, `grafo.c`, `grafo.h` y el archivo `graphquest.csv`.

### Opción 1: En Visual Studio Code
1. Inicia **Visual Studio Code**.
2. Selecciona `Archivo > Abrir carpeta...` y elige la carpeta donde descomprimiste el proyecto.
3. Abre la terminal integrada (`Terminal > Nueva terminal`).

### Opción 2: En Replit
1. Crea un nuevo repl o importa la carpeta del proyecto descomprimido.
2. Asegúrate de que el archivo graphquest.csv esté subido en la raíz del proyecto.
3. Abre la terminal de Replit (`Shell`).

### Forma de compilación del codigo

1. Compila el codigo escribiendo:
   ```bash
   gcc -O3 tarea3.c lista.c grafo.c -Wno-unused-result -o tarea3
   ```
2. Una vez compilado el programa escribe:
   ```bash
   ./tarea3
   ```

## Funcionalidades

El sistema cuenta con las siguientes opciones operativas:

**Menú principal**
1. Cargar laberinto desde archivo CSV: Lee el archivo (por defecto graphquest.csv) y construye el grafo de escenarios. Es requisito hacer esto antes de jugar.
2. Iniciar partida: Permite elegir entre el modo de un jugador o el modo de dos jugadores.
3. Salir: Finaliza la ejecución del sistema de manera segura, liberando toda la memoria utilizada por los TDAs.

**Menú durante la partida**
1. Recoger ítem(s): Permite elegir uno o más ítems del escenario actual (ej: `1 3`) y pasarlos al inventario. Descuenta 1 de tiempo.
2. Descartar ítem(s): Permite eliminar uno o más ítems del inventario para reducir el peso. Descuenta 1 de tiempo.
3. Avanzar en una dirección: Mueve al jugador por una conexión válida del escenario. Descuenta el tiempo según el peso total transportado.
4. Reiniciar partida: Vuelve al escenario inicial con inventario vacío, tiempo completo y todos los ítems en su lugar.
5. Salir del juego: Termina la partida y cierra la aplicación.

**Modo de dos jugadores (colaborativo por turnos):** cada jugador realiza 1 o 2 acciones por turno. Los ítems son únicos, por lo que si un jugador toma uno, el otro ya no lo ve. La partida termina cuando ambos llegan a la salida o se quedan sin tiempo, y si ambos llegan se muestran los dos inventarios y el puntaje total.

**Formato del archivo CSV**
```
ID,Nombre,Descripcion,Items,Arriba,Abajo,Izquierda,Derecha,EsFinal
```
- `Items`: `nombre,valor,peso` separados por `;` (puede ir vacío).
- `Arriba`, `Abajo`, `Izquierda`, `Derecha`: id del escenario destino, o `-1` si no hay salida.
- `EsFinal`: `Si` o `No`.
- El primer escenario del archivo es el escenario inicial.

## Opciones que no funcionan o tienen limitaciones

- **Formato del CSV fijo**: las columnas se leen por posición, no por el nombre del encabezado. Si el archivo tiene otro orden, el laberinto no se carga correctamente.
- **Textos largos**: las descripciones se cortan a 255 caracteres y los nombres a 49.
- **Ítems descartados**: desaparecen del juego (no quedan en el escenario), tal como indica el enunciado.
- **Tiempo insuficiente para avanzar**: si el costo del movimiento es mayor al tiempo restante, el jugador pierde en ese momento.
- **Dos jugadores**: las opciones Reiniciar y Salir afectan a toda la partida, no solo al jugador del turno.
- **Entrada por teclado**: si se escribe texto en vez de un número, se muestra "Opcion no valida" y se pide de nuevo. Las acciones canceladas o inválidas (dirección sin salida, selección vacía) no descuentan tiempo.

## Ejemplo de uso

**Inicio:** Muestra el menú principal, donde se puede acceder a todas las funciones del codigo.

```
===== GraphQuest =====
1. Cargar laberinto desde archivo CSV
2. Iniciar partida (cargue un laberinto primero)
3. Salir
Opcion:
```

**Paso 1:** El sistema lee el archivo CSV y construye el grafo. Es requisito hacer esto antes de jugar.

```
Opcion: 1
Nombre del archivo [graphquest.csv]:
Laberinto cargado: 16 escenarios
```

**Paso 2:** El usuario inicia la partida y elige el modo de juego. El juego comienza en el escenario inicial y muestra el estado del jugador.

```
Opcion: 2

Modo de juego:
1. Un jugador
2. Dos jugadores (colaborativo por turnos)
0. Volver al menu
Opcion: 1

--------------------------------------------
Estas en: Entrada principal
Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.

Items en este escenario:
  (no hay items)

Tiempo restante: 10
Inventario (peso total: 0 kg, puntaje: 0 pts):
  (vacio)

Direcciones posibles: abajo

1. Recoger item(s)
2. Descartar item(s)
3. Avanzar en una direccion
4. Reiniciar partida
5. Salir del juego
Opcion:
```

**Paso 3:** El usuario avanza en una dirección válida. El costo de tiempo depende del peso del inventario.

```
Opcion: 3
Direccion (1 arriba, 2 abajo, 3 izquierda, 4 derecha, 0 cancelar): 2
Te moviste hacia abajo (tiempo usado: 1)

--------------------------------------------
Estas en: Cocina
...
Items en este escenario:
  1. Cuchillo (3 pts, 1 kg)
  2. Pan (2 pts, 1 kg)

Tiempo restante: 9
```

**Paso 4:** El usuario recoge uno o más ítems del escenario escribiendo sus números separados por espacio.

```
Opcion: 1
Numeros de los items a recoger (ej: 1 3), 0 para cancelar: 1 2
Recogiste: Pan
Recogiste: Cuchillo

Tiempo restante: 8
Inventario (peso total: 2 kg, puntaje: 5 pts):
  1. Pan (2 pts, 1 kg)
  2. Cuchillo (3 pts, 1 kg)
```

**Paso 5:** El usuario descarta ítems de su inventario para reducir el peso.

```
Opcion: 2
Numeros de los items a descartar (ej: 1 2), 0 para cancelar: 1
Descartaste: Pan

Tiempo restante: 7
Inventario (peso total: 1 kg, puntaje: 3 pts):
  1. Cuchillo (3 pts, 1 kg)
```

**Paso 6:** Si el jugador llega al escenario final con tiempo disponible, el juego termina mostrando su inventario y el puntaje final.

```
Te moviste hacia derecha (tiempo usado: 1)

============================================
Llegaste a la salida!
Inventario (peso total: 6 kg, puntaje: 23 pts):
  1. Cuchillo (3 pts, 1 kg)
  2. Rubi (20 pts, 5 kg)
Puntaje final: 23
============================================
```

Si el tiempo se agota antes de llegar a la salida, se muestra el mensaje de derrota:

```
============================================
Se acabo el tiempo... has perdido.
============================================
```

**Paso 7:** En el modo de dos jugadores, antes de cada turno se indica qué jugador juega, y tras la primera acción se pregunta si se desea realizar una segunda.

```
######## Turno del jugador 1 ########
...
Realizar una segunda accion? (1 = si, 0 = terminar turno): 0

######## Turno del jugador 2 ########
...
```

**Paso 8:** Finaliza la ejecución del sistema, limpiando los TDAs y liberando la memoria.

```
Opcion: 3
Hasta la proxima!
```

## Contribuciones
- **Ignacio Aracena:**
  - Lógica de lectura, extracción de datos y carga desde el archivo CSV.
  - Construcción del grafo (creación de escenarios y conexiones entre ellos).
  - Implementación de las funciones de recoger y descartar ítems.
  - Modo de dos jugadores colaborativo por turnos y liberación de memoria de los TDAs.

- **Franco Gallo:**
  - Estructuración del Menú Principal y flujo de interacción con el usuario.
  - Implementación de la función de avanzar, con el cálculo del tiempo según el peso del inventario.
  - Lógica de victoria, derrota, reinicio y salida de la partida.
  - Diseño de la función de impresión estructurada del estado del jugador en pantalla (modo de un jugador).

El readme fue desarrollado por ambas partes.