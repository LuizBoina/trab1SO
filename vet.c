#include "vet.h"
pid_t *pgids;
int pgids_tam;

void inicializaVetPgids() {
    pgids_tam = 50;
    pgids = (pid_t*)malloc(sizeof(pid_t)*pgids_tam);
    for(int i = 0;pgids_tam > i; i++)
        pgids[i] = i;
}

void insereVetPgids(pid_t *pgids, pid_t pgid){
    int espacoVazio = procuraEspaco(pgids);
    if(espacoVazio == -1) {
        realocaVetor(pgids);
        pgids[pgids_tam/2] = pgid;
    }
    else
        pgids[espacoVazio] = pgid;
}

int procuraEspaco(pid_t *pgids){
    for(int i = 0; pgids_tam > i; i++){
        if(pgids[i] != 0)
            return i;
    }
    return -1;
}

void realocaVetor(pid_t *pgids) {
    pgids_tam *= 2;
    pgids = (pid_t *)realloc(pgids, sizeof(pid_t)*pgids_tam);
    for(int i = pgids_tam/2;pgids_tam > i;i++)
        pgids[i] = 0;
}

int VetVazio(pid_t *pgids){
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
}
