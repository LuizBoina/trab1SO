#include "gsh.h"
#include "lista.h"
#include "vet.h"
Vet* vetPgids;

int main() {
    Lista* comandos;
    vetPgids = inicializaVetPgids();
    setaSinais();
    int i = 0;
    while(1){ //tratar entrads (somente enter e comandos inexistentes)
        comandos = leLinha();
        pid_t pgid = criaProcessos(comandos);
        if(pgid != -1)
            insereVetPgids(vetPgids, pgid);
    }
}