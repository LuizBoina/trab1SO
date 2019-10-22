#define BACKGROUND 0
#define FOREGROUND 1
#define LEADER -1
//#define MOEDA rand() % 2
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gsh.h"
#include <string.h>
#include <time.h>
#include <signal.h>

extern pid_t pgids[];
extern int pgids_tam;

void imprimePrompt(){
    printf("ghs>");
}

Lista* leLinha(){
    Lista* comandos = NULL;
    char linha[500];
    for(int i = 0; 500> i;i++)
        linha[i] = '\0';
    imprimePrompt();
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
        if(!strcmp(comando,"mywait") || !strcmp(comando,"clean&die")) {
            operacaoInterna(comando);
            return -1;
        }
        groupid = criaProcesso(comando, FOREGROUND, LEADER);
        removePrimeiro(comandos);
    }
    else{
        while(comandos != NULL){
            char* comando = pegaPrimeiro(comandos);
            if (cont++ == 0) {
                groupid = criaProcesso(comando, BACKGROUND, LEADER);
                //impede que os outros processos sejam executados antes de criar o grupo
                nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);
            }
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
    pid_t pgid = 0;
    char* split = strtok(comando, " ");
    while(split != NULL){
        args[i++] = split;
        split = strtok(NULL, " ");
    }
    if(args[0] == NULL)
        return -1;
    args[i] = NULL;
    pid_t pid;
    if((pid = fork()) < 0) //como tratar todos os sinais que um processo recebe para matar todo o grupo
        printf("erro fork() comando: %s\n", comando);
    if(pid == 0){ /* processo filho */
        if(groupid == LEADER) {
            setpgrp();
            printf("MEU PID = %d, PID DO GRUPO = %d\n", getpid(), getpgrp());
        }
        else { /* processo pai */
            setpgid(getpid(), groupid);
            printf("MEU PID = %d, PID DO GRUPO = %d\n", getpid(), getpgrp());
        }
        int moeda = rand() % 2; //os processos estão com a mesma moeda
        printf("moeda: %d\n", moeda);
        if(moeda) { //moeda pode ser 0 ou 1, se moeda == 1 cria ghost, caso contrario, nao cria
            printf("ghost criado\n");
            pid_t pid2;
            pid2 = fork(); //como saber se é fhost (trata_SIGINT)
            if(pid2 == 0){
                printf("GHOST === MEU PID = %d, PID DO GRUPO = %d\n", getpid(), getpgrp());
            }
        }
        if(execvp(args[0], args) == -1){
            printf("erro ao executar o comando: %s\n", comando);
            return -1;
        }
    }
    else{
        if(tipo == FOREGROUND){ //travar o terminal até o filho morrer ou ser suspenso
            int status;

            int pgid = getpgid(pid);
            //printf("------pid: %d--------pgid: %d------------", pid, pgid);
            waitpid(pid, &status, WUNTRACED);
            printaPgid();
            removePgid(pgid);
            printaPgid();
            if(status){

            }
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
    signal(SIGCHLD, trata_SIGCHLD);
}

void trata_SIGINT(int signum){
    sigset_t mask;
    sigset_t oldset;
    sigfillset(&mask);
    sigemptyset(&oldset);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    printf("\nFoi Capturado um Ctrl+C (SIGINT)\n");
    if(VetVazio()) {
        char resposta[50];
        printf("Digite Y para fechar a shell ou qualquer coisa para cancelar: ");
        scanf("%s", resposta);
        if (!strcmp(resposta,"Y") || !strcmp(resposta,"y")) {
            if(kill(getpgrp(), SIGKILL) == -1)
                perror("Falha ao matar o shell");
        }
    }
    else {
        if(kill(getpgrp(), SIGKILL) == -1)
            perror("Falha ao matar o shell");
    }
    sigprocmask(SIG_SETMASK, &oldset, NULL);
}

void trata_SIGTSTP(int signum){ //parar somente os descentes da shell, ela nao
    printf("\nFoi Capturado um Ctrl+Z (SIGTSTP)\n");
    int i;
    for(i = 0; i < pgids_tam; i++) {
        if(pgids[i] != 0) {
            kill(-pgids[i], SIGTSTP);
        }
    }

}

void trata_SIGCHLD(int signum){
    int status;
    pid_t pid;
    pid = waitpid(-1, &status, WNOHANG);
    printf("\n-----------RECEBEU SIGCHLD  %d-------------\n", pid);
}

void operacaoInterna(char* comando) {
    if(!strcmp(comando,"mywait")) {
        printf("Executando o My Wait...\n");
        pid_t pid = 0;
        while ((pid = waitpid(-1, NULL, WNOHANG)) != -1) {
            printf("Liberei o zumbi PID = %d\n", pid);
        }
        printf("Liberei todos os zumbis\n");

    }
    else { // clean&die
        printf("Executando o Clean & Die...\n");
        int i;
        for(i = 0; i < pgids_tam; i++) {
            if(pgids[i] != 0) {
                printf("Matando o grupo i = %d, PGID = %d\n", i, pgids[i]);
                if(kill(-pgids[i], SIGKILL) == -1)
                    perror("Falha ao matar um grupo");
            }
        }
        printf("Agora vou me matar...\n");
        if(kill(getpgrp(), SIGKILL) == -1)
            perror("Falha ao matar a shell");
    }
}
