#ifndef TRAB1SO_LISTA_H
#define TRAB1SO_LISTA_H

typedef struct lista Lista;

//insere comando na lista
Lista *insereLista(Lista *comandos, char *comando);

//retorna o tamanho da lista
int tamLista(Lista *comandos);

//retorna o primeiro elemento da lista
char *pegaPrimeiro(Lista *comandos);

//retira o primeiro elemento da lista
Lista *removePrimeiro(Lista *comandos);

#endif //TRAB1SO_LISTA_H
