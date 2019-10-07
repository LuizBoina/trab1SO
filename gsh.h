#ifndef GSH_H
#define GSH_H
#include "lista.h"
#include <sys/types.h>

void imprimePrompt();
Lista* leLinha();
int criaProcessos(Lista* comandos);
pid_t criaProcesso(char* comando, int tipo, int groupid);

#endif /* GSH_H */