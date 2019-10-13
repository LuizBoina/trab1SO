#ifndef GSH_H
#define GSH_H
#include "lista.h"
#include <sys/types.h>
#include "vet.h"

Lista* leLinha();
pid_t criaProcessos(Lista* comandos);
pid_t criaProcesso(char* comando, int tipo, int groupid);
void setaSinais();
void trata_SIGINT(int signum);
void trata_SIGTSTP(int signum);

#endif /* GSH_H */