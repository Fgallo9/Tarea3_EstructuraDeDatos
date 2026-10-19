#ifndef GRAFO_H
#define GRAFO_H

/* TDA Grafo dirigido con lista de adyacencia.
   Cada arista lleva una direccion (arriba, abajo, izquierda, derecha). */

typedef enum {
    ARRIBA = 0,
    ABAJO,
    IZQUIERDA,
    DERECHA,
    NUM_DIRECCIONES
} Direccion;

typedef struct Arista {
    int destino;            /* id del nodo al que llega */
    Direccion dir;
    struct Arista *sig;
} Arista;

typedef struct {
    int id;
    void *dato;             /* aqui va el Escenario */
    Arista *aristas;
} NodoGrafo;

typedef struct {
    NodoGrafo *nodos;
    int cantidad;
    int capacidad;
} Grafo;

Grafo *grafoCrear(void);
int grafoAgregarNodo(Grafo *grafo, int id, void *dato);   /* 1 ok, 0 error o id repetido */
int grafoAgregarArista(Grafo *grafo, int origen, int destino, Direccion dir);
void *grafoObtenerDato(const Grafo *grafo, int id);       /* NULL si no existe */
int grafoVecino(const Grafo *grafo, int id, Direccion dir); /* id destino o -1 */
int grafoCantidadNodos(const Grafo *grafo);
void grafoDestruir(Grafo *grafo);                          /* no libera los datos */

const char *direccionATexto(Direccion dir);
int textoADireccion(const char *texto);                    /* -1 si no coincide */

#endif
