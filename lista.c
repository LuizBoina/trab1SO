#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct lista{
    char comando[80];
    Lista* prox;
};

Lista* insereLista(Lista* comandos, char* comando){
    Lista* novoComando = (Lista*)malloc(sizeof(Lista));
    strcpy(novoComando->comando, comando);
    novoComando->prox = comandos;
    return novoComando;
}

void imprimeLista(Lista* comandos){
    while(comandos != NULL){
        printf("\n%s", comandos->comando);
        comandos = comandos->prox;
    }
}

int tamLista(Lista* comandos){
    int tam;
    for(tam = 0;comandos != NULL; comandos = comandos->prox, tam++);
    return tam;
}

char* pegaPrimeiro(Lista* comandos){
    return comandos->comando;
}

Lista* removePrimeiro(Lista* comandos){
    if(comandos == NULL){
        return NULL;
    }
    Lista* removido = comandos;
    comandos = comandos->prox;
    free(removido);
    return comandos;
}