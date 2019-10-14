#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#ifndef TRAB1SO_VET_H
#define TRAB1SO_VET_H

void inicializaVetPgids();
void insereVetPgids(pid_t pgid);
void realocaVetor();
int procuraEspaco();
int VetVazio();
void printaPgid();
int VetVazio();
void removePgid(int pgid);

#endif //TRAB1SO_VET_H
