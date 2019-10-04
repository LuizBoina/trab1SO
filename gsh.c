#define BACKGROUND 0
#define FOREGROUND 1
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
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
    char delim[3] = " # ";
    char* comando = strtok(linha, delim);
    while(comando != NULL){
        comandos = insereLista(comandos, comando);
        comando = strtok(NULL, delim);
    }
    return comandos;
}

int criaProcessos(Lista* comandos){
    if(tamLista(comandos) == 1){
        criaProcesso(comando, FOREGROUND);
    }
    else{
        while(comandos != NULL){
            char* comando = pegaPrimeiro(comandos);
            criaProcesso(comando, BACKGROUND);
            removePrimeiro(comandos);
        }
    }
}

criaProcesso(comando, tipo){
    char* args[10];
    int i = 0;
    char* split = strtok(comando, " ");
    while(split != NULL){
        args[i++] = split;
        split = strtok(NULL, " ");
    }
    args[i+1] = NULL;
    pid_t pid;
    int moeda = rand() % 2; //se moeda = 0 não cria ghost caso contrário, cria
    if((pid = fork()) < 0)
        printf("erro fork() comando:%s", comando);
    if(pid == 0){
        
        execvp(args[0], args);
    }
    else{
        if(FOREGROUND){ //travar o terminal até o filho morrer ou ser suspenso
            int status;
            wait(pid, &status, WUNTRACED);
        }
    }
    if()
}