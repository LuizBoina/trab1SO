#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct lista {
    char comando[150];
    Lista *prox;
};

Lista *insereLista(Lista *comandos, char *comando) {
    Lista *novoComando = (Lista *) malloc(sizeof(Lista));
    strcpy(novoComando->comando, comando);
    novoComando->prox = comandos;
    return novoComando;
}

int tamLista(Lista *comandos) {
    int tam;
    for (tam = 0; comandos != NULL; comandos = comandos->prox, tam++);
    return tam;
}

char *pegaPrimeiro(Lista *comandos) {
    return comandos->comando;
}

Lista *removePrimeiro(Lista *comandos) {
    if (comandos == NULL) {
        return NULL;
    }
    Lista *removido = comandos;
    comandos = comandos->prox;
    free(removido);
    return comandos;
}