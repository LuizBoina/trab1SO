#include "gsh.h"
#include "lista.h"

int main() {
    Lista* comandos;
    while(1){
        comandos = leLinha();
        imprimeLista(comandos);
    }
    return 0;
}