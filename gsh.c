#define BACKGROUND 0
#define FOREGROUND 1
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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
    char *comando = strtok(linha, "#");
    while(comando!= NULL){
        comandos = insereLista(comandos, comando);
        comando = strtok(NULL, "#");
    }
    return comandos;
}

int criaProcessos(Lista* comandos){
    if(tamLista(comandos) == 1){
        char* comando = pegaPrimeiro(comandos);
        criaProcesso(comando, FOREGROUND);
        removePrimeiro(comandos);
    }
    else{
        while(comandos != NULL){
            char* comando = pegaPrimeiro(comandos);
            criaProcesso(comando, BACKGROUND);
            removePrimeiro(comandos);
        }
    }
}

void criaProcesso(char* comando, int tipo){ //checar se necessita de retorno
    char* args[10];
    int i = 0;
    char* split = strtok(comando, " ");
    while(split != NULL){
        args[i++] = split;
        split = strtok(NULL, " ");
    }
    args[i] = NULL;
    int moeda = rand() % 2; //se moeda = 0 não cria ghost caso contrário, cria
    pid_t pid, pid_ghost;
    if((pid = fork()) < 0)
        printf("erro fork() comando: %s", comando);
    if(pid == 0){
        
        if(execvp(args[0], args) == -1)
            printf("erro ao executar o comando: %s", comando);
    }
    else{
        if(tipo == FOREGROUND){ //travar o terminal até o filho morrer ou ser suspenso
            int status;
            waitpid(pid, &status, WUNTRACED);
        }
        else{

        }
    }
}