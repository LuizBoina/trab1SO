#define BACKGROUND 0
#define FOREGROUND 1
#define LEADER -1
#define MOEDA rand() % 2
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gsh.h"
#include <string.h>
#define TAM_PGIDS 200
#define PROMPT printf("\nghs>")
extern Vet* vetPgids;

Lista* leLinha(){
    Lista* comandos = NULL;
    char linha[500];
    PROMPT;
    scanf("%[^\n]", linha);
    scanf("%*c");
    char *comando = strtok(linha, "#");
    while(comando!= NULL){
        comandos = insereLista(comandos, comando);
        comando = strtok(NULL, "#");
    }
    return comandos;
}

pid_t criaProcessos(Lista* comandos){
    if(comandos == NULL)
        return -1;
    int cont = 0;
    pid_t groupid;
    if(tamLista(comandos) == 1){
        char* comando = pegaPrimeiro(comandos);
        groupid = criaProcesso(comando, FOREGROUND, LEADER);
        removePrimeiro(comandos);
    }
    else{
        while(comandos != NULL){
            char* comando = pegaPrimeiro(comandos);
            if (cont++ == 0)
                groupid = criaProcesso(comando, BACKGROUND, LEADER);
            else
                criaProcesso(comando, BACKGROUND, groupid);
            comandos = removePrimeiro(comandos);
        }
    }
    return groupid;
}

pid_t criaProcesso(char* comando, int tipo, int groupid){
    char* args[5]; //nome do executável mais três argumentos (no máximo) e NULL
    int i = 0;
    char* split = strtok(comando, " ");
    while(split != NULL){
        args[i++] = split;
        split = strtok(NULL, " ");
    }
    args[i] = NULL;
    pid_t pid;
    if((pid = fork()) < 0) //conseguir saber cmo o filho foi morto para tratar
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
        if(MOEDA) { //moeda pode ser 0 ou 1, se moeda == 1 cria ghost, caso contrario, nao cria
            printf("ghost criado\n");
            fork(); //como saber se é fhost (trata_SIGINT)
        }
        if(execvp(args[0], args) == -1){
            printf("erro ao executar o comando: %s\n", comando);
            return -1;
        }
    }
    else{
        if(tipo == FOREGROUND){ //travar o terminal até o filho morrer ou ser suspenso
            int status;
            waitpid(pid, &status, WUNTRACED);
            if(status){

            }
        }
        else{
            int status;
            wait(&status);
        }
    }
    if(groupid == LEADER) {
        groupid = pid;
    }
    return groupid;
}

void setaSinais(){
    signal(SIGINT, trata_SIGINT);
    signal(SIGTSTP, trata_SIGTSTP);
}

void trata_SIGINT(int signum){ //se tiver descendentes vivo pergunta se realmente quer fechar, se nao fecha a shell
    if(ehVetVazio(vetPgids)) {
        char resposta[100];
        printf("Digite Y para fechar a shell ou qualquer coisa para cancelar");
        scanf("%[^\n]", &resposta);
        scanf("%*c");
        if (resposta[0] != 'Y' || resposta[0] != 'y')
            return;
    }
    if(kill(getpgrp(), SIGKILL) == -1)
        perror("Falha ao matar o shell");
}

void trata_SIGTSTP(int signum){ //parar somente os descentes da shell, ela nao

}