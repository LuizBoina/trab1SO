#include "gsh.h"
#include "lista.h"
#define TAM_PGIDS 200
pid_t pgids[TAM_PGIDS]; //checar se precisa realocar

void inicializaVetpgids(pid_t *pgids);

int main() {
    Lista* comandos;
    inicializaVetpgids(pgids);
    setaSinais();
    int i = 0;
    while(1){ //tratar entrads (somente enter e comandos inexistentes)
        comandos = leLinha();
        pid_t pgid = criaProcessos(comandos);
        if(pgid != -1)
            pgids[i++] = pgid;
    }
}

void inicializaVetpgids(pid_t *pgids) {
    for(int i = 0;TAM_PGIDS>i;i++, pgids[i] = 0);
}