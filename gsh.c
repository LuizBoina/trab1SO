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
#include <time.h>
#include <signal.h>
#include <errno.h>


extern pid_t pgids[][3];
extern int pgids_tam;

void imprimePrompt(){
    printf("gsh>");
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

void criaProcessos(Lista* comandos){
    int seed;
    srand(seed);
    if(comandos == NULL)
        return;
    int cont = 0;
    pid_t pgid;
    if(tamLista(comandos) == 1){
        char* comando = pegaPrimeiro(comandos);
        if(!strcmp(comando,"mywait") || !strcmp(comando,"clean&die")) {
            operacaoInterna(comando);
            return;
        }
        pgid = criaProcesso(comando, FOREGROUND, LEADER);
        removePrimeiro(comandos);
    }
    else{
        while(comandos != NULL){
            char* comando = pegaPrimeiro(comandos);
            if (cont++ == 0) {
                pgid = criaProcesso(comando, BACKGROUND, LEADER);
                //impede que os outros processos sejam executados antes de criar o grupo
                //sleep(5);
                //nanosleep((const struct timespec[]){{0, 999999999L}}, NULL);
            }
            else
                criaProcesso(comando, BACKGROUND, pgid);
            comandos = removePrimeiro(comandos);
            srand(rand());
        }
    }
    //printf("----- groupid = %d", pgid);
}

pid_t criaProcesso(char* comando, int tipo, int groupid){
    char* args[5];
    int i = 0;
    int moeda = rand() % 2;
    char* split = strtok(comando, " ");
    while(split != NULL){
        args[i++] = split;
        split = strtok(NULL, " ");
    }
    if(args[0] == NULL)
        return -1;
    args[i] = NULL;
    pid_t pid;
    if((pid = fork()) < 0) 
        printf("erro fork() comando: %s\n", comando);
    if(pid == 0){ /* processo filho */
        if(groupid == LEADER) {
            setpgrp();
            printf("SOU O LIDER! COMANDO: %s, MEU PID = %d, PID DO GRUPO = %d\n", comando, getpid(), getpgrp());
        }
        else { /* processo pai */
            if((setpgid(getpid(), getpgrp())) == -1)
                perror("setpgid() error:");
            printf("COMANDO: %s, MEU PID = %d, PID DO GRUPO = %d\n", comando, getpid(), getpgrp());
        }
        if(moeda) {
            printf("ghost criado\n");
            pid_t pid2;
            pid2 = fork();
            if(pid2 == 0){
                printf("GHOST === COMANDO: %s, MEU PID = %d, PID DO GRUPO = %d\n", comando, getpid(), getpgrp());
            }
        }
        //signal(SIGINT, SIG_IGN);
        if(execvp(args[0], args) == -1){
            printf("\nerro ao criar comando: %s",comando);
            return groupid;
        }
    }
    else{
        if(groupid == LEADER) {
            groupid = pid;
            int time = sleep(20);
            printf("\nTEMPO RESTANTE DO SLEEP: %d\n", time);
        }
        insereVetPgids(pid, groupid, 0);
        if(tipo == FOREGROUND){
            int status;
            //printaPgid();
            waitpid(pid, &status, WUNTRACED);
            if(status > 0 && status != 5503){
                pid_t pgid = pegaPgid(pid);
                mataGrupo(pgid);
            }
            else
                removePid(pid);
            //if(status == 5503)
        }
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
    if(!checaVetSIGINT()) {
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
        pid_t pid = retornaIndex(i)[0];
        if(pid != 0) {
            printf("pgids[i][0]: %d\n", pid);
            kill(pid, SIGTSTP);
        }
    }
}

void trata_SIGCHLD(int signum){
    //printaPgid();
    int status;
    pid_t pid;
    pid = waitpid(-1, &status, WNOHANG);
    printf("\n-----------RECEBEU SIGCHLD  %d-------------\n", pid);
    if(status > 0){
        pid_t pgid = pegaPgid(pid);
        mataGrupo(pgid);
    }
    if(pid != -1)
        removePid(pid);
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
        printaPgid();
        for(int i = 0; i < pgids_tam; i++) {
            pid_t* _pgid = retornaIndex(i);
            if(_pgid[0] != 0) {
                printf("Matando o Processo i = %d, PID = %d, PGID = %d\n", i, _pgid[0], _pgid[1]);
                if(kill(-_pgid[1], SIGKILL) == -1)

                    perror("Falha ao matar um grupo");
            }
        }
        printf("Agora vou me matar...\n");
        if(kill(getpgrp(), SIGKILL) == -1)
            perror("Falha ao matar a shell");
    }
}
