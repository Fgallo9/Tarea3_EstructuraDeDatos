#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "grafo.h"

static int buscarIndice(const Grafo *grafo, int id) {
    for (int i = 0; i < grafo->cantidad; i++) {
        if (grafo->nodos[i].id == id) return i;
    }
    return -1;
}

Grafo *grafoCrear(void) {
    Grafo *grafo = (Grafo *) malloc(sizeof(Grafo));
    if (grafo == NULL) return NULL;
    grafo->capacidad = 16;
    grafo->cantidad = 0;
    grafo->nodos = (NodoGrafo *) malloc(sizeof(NodoGrafo) * grafo->capacidad);
    if (grafo->nodos == NULL) {
        free(grafo);
        return NULL;
    }
    return grafo;
}

int grafoAgregarNodo(Grafo *grafo, int id, void *dato) {
    if (buscarIndice(grafo, id) != -1) return 0;

    if (grafo->cantidad == grafo->capacidad) {
        int nuevaCap = grafo->capacidad * 2;
        NodoGrafo *tmp = (NodoGrafo *) realloc(grafo->nodos, sizeof(NodoGrafo) * nuevaCap);
        if (tmp == NULL) return 0;
        grafo->nodos = tmp;
        grafo->capacidad = nuevaCap;
    }

    NodoGrafo *nodo = &grafo->nodos[grafo->cantidad];
    nodo->id = id;
    nodo->dato = dato;
    nodo->aristas = NULL;
    grafo->cantidad++;
    return 1;
}

int grafoAgregarArista(Grafo *grafo, int origen, int destino, Direccion dir) {
    int iOrigen = buscarIndice(grafo, origen);
    if (iOrigen == -1 || buscarIndice(grafo, destino) == -1) return 0;

    /* si ya hay una arista en esa direccion, no se agrega otra */
    for (Arista *a = grafo->nodos[iOrigen].aristas; a != NULL; a = a->sig) {
        if (a->dir == dir) return 0;
    }

    Arista *nueva = (Arista *) malloc(sizeof(Arista));
    if (nueva == NULL) return 0;
    nueva->destino = destino;
    nueva->dir = dir;
    nueva->sig = grafo->nodos[iOrigen].aristas;
    grafo->nodos[iOrigen].aristas = nueva;
    return 1;
}

void *grafoObtenerDato(const Grafo *grafo, int id) {
    int i = buscarIndice(grafo, id);
    if (i == -1) return NULL;
    return grafo->nodos[i].dato;
}

int grafoVecino(const Grafo *grafo, int id, Direccion dir) {
    int i = buscarIndice(grafo, id);
    if (i == -1) return -1;
    for (Arista *a = grafo->nodos[i].aristas; a != NULL; a = a->sig) {
        if (a->dir == dir) return a->destino;
    }
    return -1;
}

int grafoCantidadNodos(const Grafo *grafo) {
    return grafo->cantidad;
}

void grafoDestruir(Grafo *grafo) {
    for (int i = 0; i < grafo->cantidad; i++) {
        Arista *a = grafo->nodos[i].aristas;
        while (a != NULL) {
            Arista *sig = a->sig;
            free(a);
            a = sig;
        }
    }
    free(grafo->nodos);
    free(grafo);
}

const char *direccionATexto(Direccion dir) {
    switch (dir) {
        case ARRIBA: return "arriba";
        case ABAJO: return "abajo";
        case IZQUIERDA: return "izquierda";
        case DERECHA: return "derecha";
        default: return "?";
    }
}

int textoADireccion(const char *texto) {
    if (strcasecmp(texto, "arriba") == 0) return ARRIBA;
    if (strcasecmp(texto, "abajo") == 0) return ABAJO;
    if (strcasecmp(texto, "izquierda") == 0) return IZQUIERDA;
    if (strcasecmp(texto, "derecha") == 0) return DERECHA;
    return -1;
}
