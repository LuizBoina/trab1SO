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

//Lê comandos digitados pelo usuário e os armazena em uma lista.
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
    int temLider = 0;
    pid_t pgid;
    if(tamLista(comandos) == 1){ //Se for passado apenas um comando.
        char* comando = pegaPrimeiro(comandos);
        if(!strcmp(comando,"mywait") || !strcmp(comando,"clean&die")) { //Trata mywait e clean&die (operação interna da shell, não é necessario fork)
            operacaoInterna(comando);
        }
        else
            pgid = criaProcesso(comando, FOREGROUND, LEADER);
        removePrimeiro(comandos);
    }
    else{ //Se for passada uma lista de comandos.
        while(comandos != NULL){
            char* comando = pegaPrimeiro(comandos);
            if (temLider++ == 0) { //Primeiro comando da lista. O PID desse comando vai ser o PGID dos processos do conjunto.
                pgid = criaProcesso(comando, BACKGROUND, LEADER);
                //impede que os outros processos sejam executados antes de criar o grupo
                //sleep(5);
                //nanosleep((const struct timespec[]){{0, 999999999L}}, NULL);
            }
            else //Outros processos do grupo que receberão o PID do primeiro processo como seu PGID.
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
    else if(pid == 0){ //Processo filho.
        if(groupid == LEADER) { //Se for o primeiro processo do conjunto.
            setpgrp(); //Cria grupo com seu PID.
            printf("SOU O LIDER! COMANDO: %s, MEU PID = %d, PID DO GRUPO = %d\n", comando, getpid(), getpgrp());
            printf("PPID: %d\n", getppid());
            kill(getppid(), SIGCONT);
        }
        else { //Se não for o primeiro processo. Vai trocar o seu PGID para o PID do primeiro processo do conjunto.
            if((setpgid(getpid(), groupid)) == -1)
                perror("setpgid() error:");
            printf("COMANDO: %s, MEU PID = %d, PID DO GRUPO = %d\n", comando, getpid(), getpgrp());
        }
        if(moeda) { //Se for para criar um ghost.
            printf("ghost criado\n");
            pid_t pid2;
            pid2 = fork();
            if(pid2 == 0){
                printf("GHOST === COMANDO: %s, MEU PID = %d, PID DO GRUPO = %d\n", comando, getpid(), getpgrp());
            }
        }
        signal(SIGINT, SIG_IGN);
        if(execvp(args[0], args) == -1){
            printf("\nerro ao criar comando: %s",comando);
            return groupid;
        }
    }
    else{ //Processo pai.
        if(groupid == LEADER) {
            pause();
            groupid = pid;
            //int time = sleep(1);
            //printf("\nTEMPO RESTANTE DO SLEEP: %d\n", time);
        }
        insereVetPgids(pid, groupid, moeda); //Insere o processo criado na matriz de processos.
        if(tipo == FOREGROUND){
            int status;
            //printaPgid();
            waitpid(pid, &status, WUNTRACED);
            if(WIFSIGNALED(status)){ //Filho foi morto por um sinal.
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
    signal(SIGCONT, trata_SIGCONT);
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
    for(int i = 0; i < pgids_tam; i++) {
        pid_t* pid = retornaIndex(i);
        if(pid[0] != 0) {
            printf("Suspendendo o grupo: PGID %d\n", pid[1]);
            if(kill(-pid[1], SIGTSTP) == -1)
                perror("Falha ao suspender");
        }
    }

}

void trata_SIGCHLD(int signum){
    //printaPgid();
    int status;
    pid_t pid;
    if((pid = waitpid(-1, &status, WNOHANG)) > 0) { //pid tem o velor de um processo.
        printf("\n-----------RECEBEU SIGCHLD  %d-------------\n", pid);
        if(WIFSIGNALED(status)){ //se o processo recebeu algum sinal.
            if(status != 20) { //se o processo não foi suspenso (SIGTSTP).
                pid_t pgid = pegaPgid(pid);
                mataGrupo(pgid);
            }
        }
    }
//    if(pid != -1)
//        removePid(pid);
}

void trata_SIGCONT(int signum) {
    printf("SHELL DESBLOQUEDA\n");
}

void operacaoInterna(char* comando) {
    if(!strcmp(comando,"mywait")) {
        printf("Executando o My Wait...\n");
        pid_t pid;
        while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
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
        if(kill(-getpgrp(), SIGKILL) == -1)
            perror("Falha ao matar a shell");
    }
}
