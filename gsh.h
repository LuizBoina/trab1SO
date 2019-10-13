#ifndef GSH_H
#define GSH_H
#include "lista.h"
#include <sys/types.h>

void imprimePrompt();
Lista* leLinha();
pid_t criaProcessos(Lista* comandos);
pid_t criaProcesso(char* comando, int tipo, int groupid);
void setaSinais();
void trata_SIGINT(int signum);
void trata_SIGTSTP(int signum);
int existeGrupo();
void operacaoInterna(char* comando);
#endif /* GSH_H */