#include "gsh.h"
#include "lista.h"
#include "vet.h"

int main() {
    Lista* comandos;
    inicializaVetPgids();
    setaSinais();
    while(1){
        comandos = leLinha();
        criaProcessos(comandos);
    }
}