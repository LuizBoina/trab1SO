#ifndef GSH_H
#define GSH_H

#include "lista.h"
#include "vet.h"

void imprimePrompt();

//le linha entrada
Lista *leLinha();

//responsavel por criar o processo de todos comando passado
void criaProcessos(Lista *comandos);

//responsavel por criar o processo de cada comando individualmente
pid_t criaProcesso(char *comando, int tipo, int groupid);

void setaSinais();

void trata_SIGINT(int signum);

void trata_SIGTSTP(int signum);

void trata_SIGCHLD(int signum);

void trata_SIGCONT(int signum);

void operacaoInterna(char *comando);

#endif /* GSH_H */