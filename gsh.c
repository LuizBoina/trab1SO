#include <stdio.h>
#include "gsh.h"
#include <string.h>
#include <stdlib.h>

void imprimePrompt(){
    printf("\nghs>");
}

void getComandos(char* comandos){
    imprimePrompt();
    scanf("%s", comandos);
}