#include <stdio.h>
#include "gsh.h"
#include <string.h>

void imprimePrompt(){
    printf("\nghs>");
}

Lista* leLinha(){
    Lista* comandos = NULL;
    char linha[400];
    imprimePrompt();
    scanf("%[^\n]%*c", linha);
    char comando[80]; int k = 0;
    for(int i = 0; strlen(linha) >= i; i++){
        if(linha[i] == '#' || linha[i] == '\0'){
            comando[k] = '\0';
            comandos = insereLista(comandos, comando);
            k = 0;
        }
        else
            comando[k++] = linha[i];
    }
    return comandos;
}
