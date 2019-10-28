#include "gsh.h"
#include "lista.h"
#include "vet.h"

int main() {
    Lista* comandos; //Lista para armazenar comandos digitados pelo usuário.
    inicializaVetPgids(); //Matriz que armazena informações sobre os processos criados.
    setaSinais(); //Tratamento de sainais.
    while(1){
        comandos = leLinha(); //Lê comandos digitados pelo usuário.
        criaProcessos(comandos); //Cria os processos especificados pelo usuário.
    }
}