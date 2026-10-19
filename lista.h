#ifndef LISTA_H
#define LISTA_H

/* TDA Lista enlazada simple (guarda punteros genericos) */

typedef struct NodoLista {
    void *dato;
    struct NodoLista *sig;
} NodoLista;

typedef struct {
    NodoLista *cabeza;
    int tamano;
} Lista;

Lista *listaCrear(void);
void listaAgregar(Lista *lista, void *dato);      /* agrega al final */
void *listaObtener(const Lista *lista, int pos);  /* NULL si pos invalida */
void *listaEliminar(Lista *lista, int pos);       /* saca el nodo y devuelve el dato */
int listaTamano(const Lista *lista);
void listaDestruir(Lista *lista, int liberarDatos);

#endif
