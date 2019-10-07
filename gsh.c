#define BACKGROUND 0
#define FOREGROUND 1
#define LEADER -1
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
    int cont = 0;
    pid_t groupid;
    
    if(tamLista(comandos) == 1){
        char* comando = pegaPrimeiro(comandos);
        criaProcesso(comando, FOREGROUND, LEADER);
        comandos = removePrimeiro(comandos);
        
    }
    else{
        while(comandos != NULL){
            char* comando = pegaPrimeiro(comandos);
            if (cont++ == 0) {
                groupid = criaProcesso(comando, BACKGROUND, LEADER);
            }
            else {
                criaProcesso(comando, BACKGROUND, groupid);
            }
            comandos = removePrimeiro(comandos);
        }
    }
}

pid_t criaProcesso(char* comando, int tipo, int groupid){ //checar se necessita de retorno
    char* args[5]; //nome do executável mais três argumentos (no máximo) e NULL
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
        printf("erro fork() comando: %s\n", comando);
    if(pid == 0){
        if(groupid == LEADER) {
            setpgrp();
            printf("MEU PID = %d, PID DO GRUPO = %d\n", getpid(), getpgrp());
        }
        else {
            setpgid(getpid(), groupid);
            printf("MEU PID = %d, PID DO GRUPO = %d\n", getpid(), getpgrp());
        }
        if(execvp(args[0], args) == -1)
            printf("erro ao executar o comando: %s", comando);
        if(moeda == 1) {
            pid_ghost = fork();
        }
    }
    else{
        if(tipo == FOREGROUND){ //travar o terminal até o filho morrer ou ser suspenso
            int status;
            waitpid(pid, &status, WUNTRACED);
        }
        else{

        }
    }
    if(groupid == -1) {
        groupid = pid;
    }
    return groupid;
}