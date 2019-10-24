#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#ifndef TRAB1SO_VET_H
#define TRAB1SO_VET_H

void inicializaVetPgids();
void insereVetPgids(pid_t pid, pid_t pgid, int ehGhost);
void realocaVetor();
int procuraEspaco();
void printaPgid();
int checaVetSIGINT();
void removePid(int pid);
void mataGrupo(int pgid);
pid_t* retornaIndex(int index);
void liberaVet();
pid_t pegaPgid(pid_t pid);

#endif //TRAB1SO_VET_H
