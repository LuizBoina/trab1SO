#ifndef GSH_H
#define GSH_H
#include "lista.h"
#include <sys/types.h>

void imprimePrompt();
Lista* leLinha();
int criaProcessos(Lista* comandos);
pid_t criaProcesso(char* comando, int tipo, int groupid);
void trata_SIGINT(int signum);
void trata_SIGTSTP(int signum);
#endif /* GSH_H */