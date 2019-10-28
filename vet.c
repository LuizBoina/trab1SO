#include "vet.h"
#include <signal.h>
#include <sys/types.h>

pid_t **pgids; //pgids[n][3] -0 pid -1 pgid -2 1 se ghost 0 caso contrario
int pgids_tam;

void inicializaVetPgids() {
    pgids_tam = 10;
    pgids = (pid_t **) malloc(sizeof(pid_t *) * pgids_tam);
    for (int i = 0; pgids_tam > i; i++)
        pgids[i] = (pid_t *) malloc(sizeof(pid_t) * 3);
    for (int j = 0; 3 > j; j++) {
        for (int i = 0; pgids_tam > i; i++)
            pgids[i][j] = 0;
    }
}

void liberaVet() {
    for (int i = 0; pgids_tam > i; i++)
        free(pgids[i]);
    free(pgids);
}

void insereVetPgids(pid_t pid, pid_t pgid, int ehGhost) {
    int espacoVazio = procuraEspaco(pgids);
    if (espacoVazio == -1) {
        realocaVetor(pgids);
        espacoVazio = pgids_tam / 2;
    }
    pgids[espacoVazio][0] = pid;
    pgids[espacoVazio][1] = pgid;
    pgids[espacoVazio][2] = ehGhost;
}

int procuraEspaco() {
    for (int i = 0; pgids_tam > i; i++) {
        if (pgids[i][0] == 0)
            return i;
    }
    return -1;
}

void realocaVetor() {
    pgids_tam *= 2;
    pgids = (pid_t **) realloc(pgids, sizeof(pid_t *) * pgids_tam);
    for (int i = pgids_tam / 2; pgids_tam > i; i++)
        pgids[i] = (pid_t *) malloc(sizeof(pid_t) * 3);
    for (int j = 0; 3 > j; j++) {
        for (int i = pgids_tam / 2; pgids_tam > i; i++)
            pgids[i][j] = 0;
    }
}

int checaVetSIGINT() {
    int i;
    for (i = 0; pgids_tam > i; i++) {
        if (pgids[i][0] != 0 && pgids[i][0] != -1)
            return 0;
    }
    return 1;
}

void removePid(int pgid) {
    for (int i = 0; pgids_tam > i; i++) {
        if (pgids[i][0] == pgid) {
            if (pgids[i][2] == 1) {
                pgids[i][0] = -1;
            } else {
                pgids[i][0] = 0;
                pgids[i][1] = 0;
                pgids[i][2] = 0;
            }
            break;
        }
    }
}

void mataGrupo(int pgid) {
    kill(-pgid, SIGKILL);
    for (int i = 0; pgids_tam > i; i++) {
        if (pgids[i][1] == pgid) {
            pgids[i][0] = 0;
            pgids[i][1] = 0;
            pgids[i][2] = 0;
        }
    }
}

pid_t *retornaIndex(int index) {
    return pgids[index];
}

pid_t pegaPgid(pid_t pid) {
    for (int i = 0; pgids_tam > i; i++) {
        if (pgids[i][0] == pid)
            return pgids[i][1];
    }
}