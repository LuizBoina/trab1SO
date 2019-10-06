#include "lista.h"
#ifndef GSH_H
#define GSH_H

void imprimePrompt();
Lista* leLinha();
int criaProcessos(Lista* comandos);
void criaProcesso(char* comando, int tipo);

#endif /* GSH_H */