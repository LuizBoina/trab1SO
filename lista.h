#ifndef TRAB1SO_LISTA_H
#define TRAB1SO_LISTA_H

typedef struct lista Lista;

Lista* insereLista(Lista* comandos, char* comando);
void imprimeLista(Lista* comandos);
int tamLista(Lista* comandos);
char* pegaPrimeiro(Lista* comandos);
Lista* removePrimeiro(Lista* comandos);

#endif //TRAB1SO_LISTA_H
