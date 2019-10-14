#include "gsh.h"
#include "lista.h"
#include "vet.h"
extern pid_t pgids[];
extern int pgids_tam;

int main() {
    Lista* comandos;
    inicializaVetPgids();
    setaSinais();
    while(1){ //tratar entrads (somente enter e comandos inexistentes)
        comandos = leLinha();
        pid_t pgid = criaProcessos(comandos);
        if(pgid != -1) {
            insereVetPgids(pgid);
            printaPgid();
        }
    }
}