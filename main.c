#include "gsh.h"
#include "lista.h"

int main() {
    Lista* comandos;
    pid_t pgids[200]; //checar se precisa realocar
    setaSinais();
    int i = 0;
    while(1){ //tratar entrads (somente enter e comandos inexistentes)
        comandos = leLinha();
        pid_t pgid = criaProcessos(comandos);
        if(pgid != -1)
            pgids[i++] = pgid;
    }
}