#ifndef GSH_H
#define GSH_H
#include "lista.h"
#include "vet.h"
#include <sys/types.h>

void imprimePrompt();
Lista* leLinha();
void criaProcessos(Lista* comandos);
pid_t criaProcesso(char* comando, int tipo, int groupid);
void setaSinais();
void trata_SIGINT(int signum);
void trata_SIGTSTP(int signum);
void trata_SIGCHLD(int signum);
void trata_SIGCONT(int signum);
void operacaoInterna(char* comando);
#endif /* GSH_H */