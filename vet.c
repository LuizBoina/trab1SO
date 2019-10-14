#include "vet.h"
pid_t *pgids;
int pgids_tam;

void inicializaVetPgids() {
    pgids_tam = 50;
    pgids = (pid_t*)malloc(sizeof(pid_t)*pgids_tam);
    for(int i = 0;pgids_tam > i; i++)
        pgids[i] = 0;
}

void insereVetPgids(pid_t pgid){
    int espacoVazio = procuraEspaco(pgids);
    if(espacoVazio == -1) {
        realocaVetor(pgids);
        pgids[pgids_tam/2] = pgid;
    }
    else
        pgids[espacoVazio] = pgid;
}

int procuraEspaco(){
    for(int i = 0; pgids_tam > i; i++){
        if(pgids[i] == 0)
            return i;
    }
    return -1;
}

void realocaVetor() {
    pgids_tam *= 2;
    pgids = (pid_t *)realloc(pgids, sizeof(pid_t)*pgids_tam);
    for(int i = pgids_tam/2;pgids_tam > i;i++)
        pgids[i] = 0;
}

int VetVazio(){
    int i;
    for(i = 0; pgids_tam > i && pgids[i] == 0; i++);
    return !i;
}

void printaPgid() {
    pid_t *bla;
    bla = pgids;
    printf("entro printf\n");
    for(int i = 0; pgids_tam > i; i++){
        int t = bla[i];
        printf("%d ", t);
    }
    printf("\n");
}

void removePgid(int pgid){
    for(int i = 0; pgids_tam>i; i++){
        if(pgids[i] == pgid){
            pgids[i] = 0;
        }
    }
}
