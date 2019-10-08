#include "gsh.h"
#include "lista.h"
#include <stdio.h>

int main() {
    Lista* comandos;
    signal(SIGNINT, trata_SIGINT);
    signal(SIGTSTP, trata_SIGTSTP);
    while(1){ //tratar entrads (somente enter e comandos inexistentes)
        sleep(1);
        comandos = leLinha();
        //imprimeLista(comandos);
        criaProcessos(comandos);
    }
    return 0;
}