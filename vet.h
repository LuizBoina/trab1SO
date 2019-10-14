#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#ifndef TRAB1SO_VET_H
#define TRAB1SO_VET_H

void inicializaVetPgids();
void insereVetPgids(pid_t *vet, pid_t pgid);
void realocaVetor(pid_t* vet);
int procuraEspaco(pid_t* vet);
int VetVazio(pid_t* vet);
int retornaTamanho(pid_t* vet);
pid_t* retornaVetPgids(pid_t* vet);
void printaPgid();

#endif //TRAB1SO_VET_H
