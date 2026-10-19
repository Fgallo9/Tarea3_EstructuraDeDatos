#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"
#include "grafo.h"

/* ================= Tipos del juego ================= */

#define MAX_NOMBRE 50
#define MAX_DESCRIPCION 256
#define TIEMPO_INICIAL 10

#define JUGANDO 0
#define EN_SALIDA 1
#define SIN_TIEMPO 2

typedef struct {
    char nombre[MAX_NOMBRE];
    int peso;
    int valor;
} Item;

typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    char descripcion[MAX_DESCRIPCION];
    Lista *items;
    int esFinal;
} Escenario;

typedef struct {
    int escenarioActual;
    int tiempoRestante;
    Lista *inventario;
    int pesoTotal;
    int puntaje;
    int estado;
} Jugador;

#define PARTIDA_TERMINADA 0
#define PARTIDA_REINICIAR 1
#define PARTIDA_SALIR     2

/* ================= Carga del laberinto (CSV) ================= */

#define MAX_LINEA 2048
#define MAX_CAMPOS 16
#define NUM_COLUMNAS 9

#define COL_ID 0
#define COL_NOMBRE 1
#define COL_DESCRIPCION 2
#define COL_ITEMS 3
#define COL_PRIMERA_DIRECCION 4
#define COL_ES_FINAL 8

void recortar(char *s) {
    char *ini = s;
    while (*ini == ' ' || *ini == '\t') ini++;
    memmove(s, ini, strlen(ini) + 1);
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '\r' || s[n - 1] == '\n'))
        s[--n] = '\0';
}

int separarCampos(char *linea, char *campos[], int max) {
    int n = 0;
    char *p = linea;
    while (n < max) {
        if (*p == '"') {
            p++;
            char *dst = p;
            campos[n] = p;
            while (*p) {
                if (*p == '"' && p[1] == '"') { *dst++ = '"'; p += 2; }
                else if (*p == '"') { p++; break; }
                else *dst++ = *p++;
            }
            *dst = '\0';
            while (*p && *p != ',') p++;
        } else {
            campos[n] = p;
            while (*p && *p != ',') p++;
        }
        n++;
        if (*p == ',') { *p = '\0'; p++; }
        else break;
    }
    return n;
}

void copiarTexto(char *destino, const char *origen, size_t tam) {
    strncpy(destino, origen, tam - 1);
    destino[tam - 1] = '\0';
}

Escenario *crearEscenario(char *campos[]) {
    Escenario *e = (Escenario *) malloc(sizeof(Escenario));
    if (e == NULL) return NULL;
    e->items = listaCrear();
    if (e->items == NULL) { free(e); return NULL; }
    e->id = atoi(campos[COL_ID]);
    copiarTexto(e->nombre, campos[COL_NOMBRE], MAX_NOMBRE);
    copiarTexto(e->descripcion, campos[COL_DESCRIPCION], MAX_DESCRIPCION);
    char c = campos[COL_ES_FINAL][0];
    e->esFinal = (c == 'S' || c == 's' || c == '1' || c == 'T' || c == 't');
    return e;
}

void liberarEscenario(Escenario *e) {
    listaDestruir(e->items, 1);
    free(e);
}

void cargarItems(Escenario *e, char *texto) {
    char *tok = texto;
    while (tok != NULL && *tok != '\0') {
        char *fin = strchr(tok, ';');
        if (fin != NULL) *fin = '\0';
        char nombre[MAX_NOMBRE];
        int valor, peso;
        if (sscanf(tok, " %49[^,],%d,%d", nombre, &valor, &peso) == 3) {
            Item *item = (Item *) malloc(sizeof(Item));
            if (item != NULL) {
                recortar(nombre);
                copiarTexto(item->nombre, nombre, MAX_NOMBRE);
                item->valor = valor;
                item->peso = peso;
                listaAgregar(e->items, item);
            }
        }
        tok = (fin != NULL) ? fin + 1 : NULL;
    }
}

Grafo *cargarLaberinto(const char *archivo, int *idInicial) {
    FILE *f = fopen(archivo, "r");
    if (f == NULL) {
        printf("No se pudo abrir el archivo %s\n", archivo);
        return NULL;
    }
    Grafo *grafo = grafoCrear();
    if (grafo == NULL) { fclose(f); return NULL; }

    char linea[MAX_LINEA];
    char *campos[MAX_CAMPOS];

    if (fgets(linea, MAX_LINEA, f) == NULL) {
        fclose(f);
        grafoDestruir(grafo);
        return NULL;
    }

    /* escenarios e items */
    while (fgets(linea, MAX_LINEA, f) != NULL) {
        if (separarCampos(linea, campos, MAX_CAMPOS) < NUM_COLUMNAS) continue;
        for (int i = 0; i < NUM_COLUMNAS; i++) recortar(campos[i]);

        Escenario *e = crearEscenario(campos);
        if (e == NULL) continue;
        if (!grafoAgregarNodo(grafo, e->id, e)) {
            liberarEscenario(e);
            continue;
        }
        if (grafoCantidadNodos(grafo) == 1) *idInicial = e->id;
        cargarItems(e, campos[COL_ITEMS]);
    }

    /* conexiones (los destinos ya existen) */
    rewind(f);
    if (fgets(linea, MAX_LINEA, f) != NULL) {
        while (fgets(linea, MAX_LINEA, f) != NULL) {
            if (separarCampos(linea, campos, MAX_CAMPOS) < NUM_COLUMNAS) continue;
            int id = atoi(campos[COL_ID]);
            for (int d = 0; d < NUM_DIRECCIONES; d++) {
                char *destino = campos[COL_PRIMERA_DIRECCION + d];
                recortar(destino);
                if (destino[0] != '\0' && atoi(destino) != -1)
                    grafoAgregarArista(grafo, id, atoi(destino), (Direccion) d);
            }
        }
    }
    fclose(f);

    if (grafoCantidadNodos(grafo) == 0) {
        grafoDestruir(grafo);
        return NULL;
    }
    return grafo;
}

void liberarLaberinto(Grafo *grafo) {
    if (grafo == NULL) return;
    for (int i = 0; i < grafo->cantidad; i++)
        liberarEscenario((Escenario *) grafo->nodos[i].dato);
    grafoDestruir(grafo);
}

/* ================= Logica de la partida ================= */

/* ---------- jugador ---------- */

Jugador *crearJugador(int idInicial) {
    Jugador *jugador = (Jugador *) malloc(sizeof(Jugador));
    if (jugador == NULL) return NULL;
    jugador->escenarioActual = idInicial;
    jugador->tiempoRestante = TIEMPO_INICIAL;
    jugador->inventario = listaCrear();
    jugador->pesoTotal = 0;
    jugador->puntaje = 0;
    jugador->estado = JUGANDO;
    return jugador;
}

void destruirJugador(Jugador *jugador) {
    listaDestruir(jugador->inventario, 1);
    free(jugador);
}

/* ---------- entrada por teclado ---------- */

void leerLinea(char *buffer, int tam) {
    if (fgets(buffer, tam, stdin) == NULL) {
        printf("\nSe cerro la entrada, saliendo...\n");
        exit(0);
    }
}

int pedirNumero(void) {
    char buffer[64];
    int numero;
    leerLinea(buffer, sizeof(buffer));
    if (sscanf(buffer, "%d", &numero) != 1) return -1;
    return numero;
}

int pedirSeleccion(int total, int *elegidos) {
    char linea[256];
    int cantidad = 0;
    leerLinea(linea, sizeof(linea));

    char *pos = linea;
    char *fin;
    while (1) {
        long n = strtol(pos, &fin, 10);
        if (fin == pos) break;
        pos = fin;
        if (n < 1 || n > total) continue;
        int repetido = 0;
        for (int i = 0; i < cantidad; i++)
            if (elegidos[i] == n - 1) repetido = 1;
        if (!repetido) elegidos[cantidad++] = (int) (n - 1);
    }
    return cantidad;
}

/* de mayor a menor, para eliminar sin que se corran las posiciones */
void ordenarDescendente(int *arreglo, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1 - i; j++)
            if (arreglo[j] < arreglo[j + 1]) {
                int aux = arreglo[j];
                arreglo[j] = arreglo[j + 1];
                arreglo[j + 1] = aux;
            }
}

/* ---------- mostrar informacion ---------- */

void mostrarInventario(const Jugador *jugador) {
    printf("Inventario (peso total: %d kg, puntaje: %d pts):\n",
           jugador->pesoTotal, jugador->puntaje);
    if (listaTamano(jugador->inventario) == 0) {
        printf("  (vacio)\n");
        return;
    }
    for (int i = 0; i < listaTamano(jugador->inventario); i++) {
        Item *item = (Item *) listaObtener(jugador->inventario, i);
        printf("  %d. %s (%d pts, %d kg)\n", i + 1, item->nombre, item->valor, item->peso);
    }
}

void mostrarEstado(const Grafo *grafo, const Jugador *jugador) {
    Escenario *esc = (Escenario *) grafoObtenerDato(grafo, jugador->escenarioActual);

    printf("\n--------------------------------------------\n");
    printf("Estas en: %s\n%s\n\n", esc->nombre, esc->descripcion);

    printf("Items en este escenario:\n");
    if (listaTamano(esc->items) == 0) printf("  (no hay items)\n");
    for (int i = 0; i < listaTamano(esc->items); i++) {
        Item *item = (Item *) listaObtener(esc->items, i);
        printf("  %d. %s (%d pts, %d kg)\n", i + 1, item->nombre, item->valor, item->peso);
    }

    printf("\nTiempo restante: %d\n", jugador->tiempoRestante);
    mostrarInventario(jugador);

    printf("\nDirecciones posibles:");
    int hay = 0;
    for (int d = 0; d < NUM_DIRECCIONES; d++)
        if (grafoVecino(grafo, jugador->escenarioActual, (Direccion) d) != -1) {
            printf(" %s", direccionATexto((Direccion) d));
            hay = 1;
        }
    if (!hay) printf(" ninguna");
    printf("\n");
}

void mostrarMenuAcciones(void) {
    printf("\n1. Recoger item(s)\n");
    printf("2. Descartar item(s)\n");
    printf("3. Avanzar en una direccion\n");
    printf("4. Reiniciar partida\n");
    printf("5. Salir del juego\n");
    printf("Opcion: ");
}

/* ---------- acciones ---------- */

void gastarTiempo(Jugador *jugador, int cantidad) {
    jugador->tiempoRestante -= cantidad;
    if (jugador->tiempoRestante <= 0 && jugador->estado == JUGANDO) {
        jugador->tiempoRestante = 0;
        jugador->estado = SIN_TIEMPO;
    }
}

int recogerItems(Grafo *grafo, Jugador *jugador) {
    Escenario *esc = (Escenario *) grafoObtenerDato(grafo, jugador->escenarioActual);
    int total = listaTamano(esc->items);
    if (total == 0) {
        printf("No hay items para recoger aqui\n");
        return 0;
    }

    printf("Numeros de los items a recoger (ej: 1 3), 0 para cancelar: ");
    int *elegidos = (int *) malloc(sizeof(int) * total);
    if (elegidos == NULL) return 0;
    int cantidad = pedirSeleccion(total, elegidos);
    if (cantidad == 0) {
        printf("No se recogio nada\n");
        free(elegidos);
        return 0;
    }

    ordenarDescendente(elegidos, cantidad);
    for (int i = 0; i < cantidad; i++) {
        Item *item = (Item *) listaEliminar(esc->items, elegidos[i]);
        listaAgregar(jugador->inventario, item);
        jugador->pesoTotal += item->peso;
        jugador->puntaje += item->valor;
        printf("Recogiste: %s\n", item->nombre);
    }
    free(elegidos);
    gastarTiempo(jugador, 1);
    return 1;
}

int descartarItems(Jugador *jugador) {
    int total = listaTamano(jugador->inventario);
    if (total == 0) {
        printf("No tienes items para descartar\n");
        return 0;
    }

    printf("Numeros de los items a descartar (ej: 1 2), 0 para cancelar: ");
    int *elegidos = (int *) malloc(sizeof(int) * total);
    if (elegidos == NULL) return 0;
    int cantidad = pedirSeleccion(total, elegidos);
    if (cantidad == 0) {
        printf("No se descarto nada\n");
        free(elegidos);
        return 0;
    }

    ordenarDescendente(elegidos, cantidad);
    for (int i = 0; i < cantidad; i++) {
        Item *item = (Item *) listaEliminar(jugador->inventario, elegidos[i]);
        jugador->pesoTotal -= item->peso;
        jugador->puntaje -= item->valor;
        printf("Descartaste: %s\n", item->nombre);
        free(item);
    }
    free(elegidos);
    gastarTiempo(jugador, 1);
    return 1;
}

int avanzar(Grafo *grafo, Jugador *jugador) {
    printf("Direccion (1 arriba, 2 abajo, 3 izquierda, 4 derecha, 0 cancelar): ");
    int opcion = pedirNumero();

    if (opcion == 0) return 0;
    if (opcion < 1 || opcion > NUM_DIRECCIONES) {
        printf("Direccion no valida\n");
        return 0;
    }

    Direccion dir = (Direccion) (opcion - 1);
    int destino = grafoVecino(grafo, jugador->escenarioActual, dir);
    if (destino == -1) {
        printf("No hay salida hacia %s\n", direccionATexto(dir));
        return 0;
    }

    /* costo = techo((peso + 1) / 10) */
    int costo = (jugador->pesoTotal + 1 + 9) / 10;
    if (costo > jugador->tiempoRestante) {
        printf("Cargas demasiado peso y el tiempo no alcanza para llegar (costo %d)\n", costo);
        jugador->tiempoRestante = 0;
        jugador->estado = SIN_TIEMPO;
        return 1;
    }

    jugador->escenarioActual = destino;
    Escenario *esc = (Escenario *) grafoObtenerDato(grafo, destino);
    printf("Te moviste hacia %s (tiempo usado: %d)\n", direccionATexto(dir), costo);

    if (esc->esFinal) {
        jugador->tiempoRestante -= costo;
        jugador->estado = EN_SALIDA;
    } else {
        gastarTiempo(jugador, costo);
    }
    return 1;
}

int ejecutarAccion(Grafo *grafo, Jugador *jugador, int opcion) {
    if (opcion == 1) return recogerItems(grafo, jugador);
    if (opcion == 2) return descartarItems(jugador);
    return avanzar(grafo, jugador);
}

/* ---------- un jugador ---------- */

int jugarSolo(Grafo *grafo, int idInicial) {
    Jugador *jugador = crearJugador(idInicial);
    if (jugador == NULL) return PARTIDA_TERMINADA;
    int resultado = PARTIDA_TERMINADA;

    while (jugador->estado == JUGANDO) {
        mostrarEstado(grafo, jugador);
        mostrarMenuAcciones();
        int opcion = pedirNumero();

        if (opcion >= 1 && opcion <= 3) {
            ejecutarAccion(grafo, jugador, opcion);
        } else if (opcion == 4) {
            resultado = PARTIDA_REINICIAR;
            break;
        } else if (opcion == 5) {
            resultado = PARTIDA_SALIR;
            break;
        } else {
            printf("Opcion no valida\n");
        }
    }

    if (resultado == PARTIDA_TERMINADA) {
        printf("\n============================================\n");
        if (jugador->estado == EN_SALIDA) {
            printf("Llegaste a la salida!\n");
            mostrarInventario(jugador);
            printf("Puntaje final: %d\n", jugador->puntaje);
        } else {
            printf("Se acabo el tiempo... has perdido.\n");
        }
        printf("============================================\n");
    }

    destruirJugador(jugador);
    return resultado;
}

/* ---------- dos jugadores (por turnos) ---------- */

int quedaAlguienJugando(Jugador *jugadores[2]) {
    return jugadores[0]->estado == JUGANDO || jugadores[1]->estado == JUGANDO;
}

int jugarDos(Grafo *grafo, int idInicial) {
    Jugador *jugadores[2];
    jugadores[0] = crearJugador(idInicial);
    jugadores[1] = crearJugador(idInicial);
    int resultado = PARTIDA_TERMINADA;
    int turno = 0;

    while (resultado == PARTIDA_TERMINADA && quedaAlguienJugando(jugadores)) {
        if (jugadores[turno]->estado != JUGANDO) {
            turno = 1 - turno;
            continue;
        }

        Jugador *jugador = jugadores[turno];
        printf("\n######## Turno del jugador %d ########\n", turno + 1);

        int acciones = 0;
        while (acciones < 2 && jugador->estado == JUGANDO) {
            mostrarEstado(grafo, jugador);
            mostrarMenuAcciones();
            int opcion = pedirNumero();

            if (opcion >= 1 && opcion <= 3) {
                if (ejecutarAccion(grafo, jugador, opcion)) acciones++;
            } else if (opcion == 4) {
                resultado = PARTIDA_REINICIAR;
                break;
            } else if (opcion == 5) {
                resultado = PARTIDA_SALIR;
                break;
            } else {
                printf("Opcion no valida\n");
                continue;
            }

            if (acciones == 1 && jugador->estado == JUGANDO) {
                printf("\nRealizar una segunda accion? (1 = si, 0 = terminar turno): ");
                if (pedirNumero() != 1) break;
            }
        }
        turno = 1 - turno;
    }

    if (resultado == PARTIDA_TERMINADA) {
        printf("\n============================================\n");
        for (int i = 0; i < 2; i++) {
            printf("Jugador %d: %s\n", i + 1,
                   jugadores[i]->estado == EN_SALIDA ? "llego a la salida" : "se quedo sin tiempo");
            mostrarInventario(jugadores[i]);
        }
        if (jugadores[0]->estado == EN_SALIDA && jugadores[1]->estado == EN_SALIDA) {
            printf("\nAmbos llegaron a la salida!\n");
            printf("Puntaje total: %d\n", jugadores[0]->puntaje + jugadores[1]->puntaje);
        } else {
            printf("\nNo lograron salir los dos... han perdido.\n");
        }
        printf("============================================\n");
    }

    destruirJugador(jugadores[0]);
    destruirJugador(jugadores[1]);
    return resultado;
}

/* ---------- punto de entrada ---------- */

int jugarPartida(Grafo *grafo, int idInicial) {
    while (1) {
        printf("\nModo de juego:\n");
        printf("1. Un jugador\n");
        printf("2. Dos jugadores (colaborativo por turnos)\n");
        printf("0. Volver al menu\n");
        printf("Opcion: ");
        int modo = pedirNumero();

        if (modo == 0) return PARTIDA_TERMINADA;
        if (modo == 1) return jugarSolo(grafo, idInicial);
        if (modo == 2) return jugarDos(grafo, idInicial);
        printf("Opcion no valida\n");
    }
}

/* ================= Menu principal ================= */

#define ARCHIVO_LABERINTO "graphquest.csv"

int leerOpcion(void) {
    char buffer[64];
    int opcion;
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return 3;
    if (sscanf(buffer, "%d", &opcion) != 1) return -1;
    return opcion;
}

void mostrarMenu(int cargado) {
    printf("\n===== GraphQuest =====\n");
    printf("1. Cargar laberinto desde archivo CSV\n");
    printf("2. Iniciar partida%s\n", cargado ? "" : " (cargue un laberinto primero)");
    printf("3. Salir\n");
    printf("Opcion: ");
}

Grafo *pedirYCargar(int *idInicial, char *nombreArchivo) {
    char nombre[256];
    printf("Nombre del archivo [%s]: ", ARCHIVO_LABERINTO);
    if (fgets(nombre, sizeof(nombre), stdin) == NULL) return NULL;
    nombre[strcspn(nombre, "\r\n")] = '\0';
    if (nombre[0] == '\0') strcpy(nombre, ARCHIVO_LABERINTO);

    Grafo *grafo = cargarLaberinto(nombre, idInicial);
    if (grafo != NULL) {
        printf("Laberinto cargado: %d escenarios\n", grafoCantidadNodos(grafo));
        strcpy(nombreArchivo, nombre);
    }
    return grafo;
}

int main(void) {
    Grafo *grafo = NULL;
    char archivoActual[256] = ARCHIVO_LABERINTO;
    int idInicial = 0;
    int salir = 0;

    while (!salir) {
        mostrarMenu(grafo != NULL);
        int opcion = leerOpcion();

        if (opcion == 1) {
            Grafo *nuevo = pedirYCargar(&idInicial, archivoActual);
            if (nuevo != NULL) {
                liberarLaberinto(grafo);
                grafo = nuevo;
            }
        } else if (opcion == 2) {
            if (grafo == NULL) {
                printf("Primero hay que cargar un laberinto (opcion 1)\n");
                continue;
            }
            
            int resultado;
            do {
                resultado = jugarPartida(grafo, idInicial);
                liberarLaberinto(grafo);
                grafo = cargarLaberinto(archivoActual, &idInicial);
                if (grafo == NULL) {
                    printf("No se pudo recargar el laberinto\n");
                    resultado = PARTIDA_SALIR;
                }
            } while (resultado == PARTIDA_REINICIAR);

            if (resultado == PARTIDA_SALIR) salir = 1;
        } else if (opcion == 3) {
            salir = 1;
        } else {
            printf("Opcion no valida\n");
        }
    }

    liberarLaberinto(grafo);
    printf("Hasta la proxima!\n");
    return 0;
}