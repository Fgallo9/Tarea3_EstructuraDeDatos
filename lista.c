#include <stdlib.h>
#include "lista.h"

Lista *listaCrear(void) {
    Lista *lista = (Lista *) malloc(sizeof(Lista));
    if (lista == NULL) return NULL;
    lista->cabeza = NULL;
    lista->tamano = 0;
    return lista;
}

void listaAgregar(Lista *lista, void *dato) {
    NodoLista *nuevo = (NodoLista *) malloc(sizeof(NodoLista));
    if (nuevo == NULL) return;
    nuevo->dato = dato;
    nuevo->sig = NULL;

    if (lista->cabeza == NULL) {
        lista->cabeza = nuevo;
    } else {
        NodoLista *aux = lista->cabeza;
        while (aux->sig != NULL) aux = aux->sig;
        aux->sig = nuevo;
    }
    lista->tamano++;
}

void *listaObtener(const Lista *lista, int pos) {
    if (pos < 0 || pos >= lista->tamano) return NULL;
    NodoLista *aux = lista->cabeza;
    for (int i = 0; i < pos; i++) aux = aux->sig;
    return aux->dato;
}

void *listaEliminar(Lista *lista, int pos) {
    if (pos < 0 || pos >= lista->tamano) return NULL;

    NodoLista *aEliminar;
    if (pos == 0) {
        aEliminar = lista->cabeza;
        lista->cabeza = aEliminar->sig;
    } else {
        NodoLista *anterior = lista->cabeza;
        for (int i = 0; i < pos - 1; i++) anterior = anterior->sig;
        aEliminar = anterior->sig;
        anterior->sig = aEliminar->sig;
    }
    void *dato = aEliminar->dato;
    free(aEliminar);
    lista->tamano--;
    return dato;
}

int listaTamano(const Lista *lista) {
    return lista->tamano;
}

void listaDestruir(Lista *lista, int liberarDatos) {
    NodoLista *aux = lista->cabeza;
    while (aux != NULL) {
        NodoLista *siguiente = aux->sig;
        if (liberarDatos) free(aux->dato);
        free(aux);
        aux = siguiente;
    }
    free(lista);
}
