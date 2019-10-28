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

extern pid_t pgids[][3];
extern int pgids_tam;

void imprimePrompt() {
    printf("gsh>");
}

//Lê comandos digitados pelo usuário e os armazena em uma lista.
Lista *leLinha() {
    Lista *comandos = NULL;
    char linha[500];
    for (int i = 0; 500 > i; i++) //limpa buffer
        linha[i] = '\0';
    imprimePrompt();
    scanf("%[^\n]", linha);
    scanf("%*c");
    char *comando = strtok(linha, "#");
    while (comando != NULL) {
        comandos = insereLista(comandos, comando);
        comando = strtok(NULL, "#");
    }
    return comandos;
}

void criaProcessos(Lista *comandos) {
    if (comandos == NULL)
        return;
    srand(time(NULL));
    int temLider = 0;
    pid_t pgid;
    if (tamLista(comandos) == 1) { //Se for passado apenas um comando.
        char *comando = pegaPrimeiro(comandos);
        if (!strcmp(comando, "mywait") ||
            !strcmp(comando, "clean&die"))//Trata mywait e clean&die (operação interna da shell, não é necessario fork)
            operacaoInterna(comando);
        else
            pgid = criaProcesso(comando, FOREGROUND, LEADER);
        removePrimeiro(comandos);
    } else { //Se for passada uma lista de comandos.
        while (comandos != NULL) {
            char *comando = pegaPrimeiro(comandos);
            if (temLider ==
                0) { //Primeiro comando da lista. O PID desse comando vai ser o PGID dos processos do conjunto.
                pgid = criaProcesso(comando, BACKGROUND, LEADER);
                temLider = 1;
            } else //Outros processos do grupo que receberão o PID do primeiro processo como seu PGID.
                criaProcesso(comando, BACKGROUND, pgid);
            comandos = removePrimeiro(comandos);
        }
    }
}

pid_t criaProcesso(char *comando, int tipo, int groupid) {
    char *args[5];
    int i = 0;
    int moeda = rand() % 2;
    char *split = strtok(comando, " ");
    while (split != NULL) {
        args[i++] = split;
        split = strtok(NULL, " ");
    }
    if (args[0] == NULL)
        return -1;
    args[i] = NULL;
    pid_t pid;
    if ((pid = fork()) < 0)
        printf("erro fork() comando: %s\n", comando);
    else if (pid == 0) { //Processo filho.
        if (groupid == LEADER) { //Se for o primeiro processo do conjunto.
            setpgrp(); //Cria grupo com seu PID.
            kill(getppid(), SIGCONT);
        } else { //Se não for o primeiro processo. Vai trocar o seu PGID para o PID do primeiro processo do conjunto.
            if ((setpgid(getpid(), groupid)) == -1)
                perror("setpgid() error:");
        }
        if (moeda) { //Se for para criar um ghost.
            pid_t pid2;
            if ((pid2 = fork()) < 0)
                printf("erro fork() do ghost comando: %s\n", comando);
        }
        signal(SIGINT, SIG_IGN); //processos ignoraram SIGINT
        if (execvp(args[0], args) == -1) {
            printf("\nerro ao criar comando: %s", comando);
            return groupid;
        }
    } else { //Processo pai.
        if (groupid == LEADER) {
            pause();
            groupid = pid;
        }
        insereVetPgids(pid, groupid, moeda); //Insere o processo criado na matriz de processos.
        if (tipo == FOREGROUND) {
            int status;
            waitpid(pid, &status, WUNTRACED);
            if (WIFSIGNALED(status)) { //Filho foi morto por um sinal.
                pid_t pgid = pegaPgid(pid);
                mataGrupo(pgid);
            } else
                removePid(pid);
        }
    }
    return groupid;
}

void setaSinais() {
    signal(SIGINT, trata_SIGINT);
    signal(SIGTSTP, trata_SIGTSTP);
    signal(SIGCHLD, trata_SIGCHLD);
    signal(SIGCONT, trata_SIGCONT);
}

void trata_SIGINT(int signum) {
    printf("\nFoi Capturado um Ctrl+C (SIGTINT)\n");
    sigset_t mask;
    sigset_t oldset;
    sigfillset(&mask);
    sigemptyset(&oldset);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    if (!checaVetSIGINT()) {
        char resposta[50];
        printf("Digite Y para fechar a shell ou qualquer coisa para cancelar: ");
        scanf("%s", resposta);
        if (!strcmp(resposta, "Y") || !strcmp(resposta, "y")) {
            liberaVet();
            if (kill(getpgrp(), SIGKILL) == -1)
                perror("Falha ao matar o shell");
        }
    } else {
        liberaVet();
        if (kill(getpgrp(), SIGKILL) == -1)
            perror("Falha ao matar o shell");
    }
    sigprocmask(SIG_SETMASK, &oldset, NULL);
}

void trata_SIGTSTP(int signum) { //parar somente os descentes da shell, ela nao
    printf("\nFoi Capturado um Ctrl+Z (SIGTSTP)\n");
    for (int i = 0; i < pgids_tam; i++) {
        pid_t *pid = retornaIndex(i);
        if (pid[0] != 0) {
            if (kill(-pid[1], SIGTSTP) == -1)
                perror("Falha ao suspender");
        }
    }

}

void trata_SIGCHLD(int signum) {
    int status;
    pid_t pid;
    if ((pid = waitpid(-1, &status, WNOHANG)) > 0) { //pid tem o velor de um processo.
        if (WIFSIGNALED(status)) { //se o processo recebeu algum sinal.
            if (status != 20) { //se o processo não foi suspenso (SIGTSTP).
                pid_t pgid = pegaPgid(pid);
                mataGrupo(pgid);
            }
        }
    }
}

void trata_SIGCONT(int signum) {
}

void operacaoInterna(char *comando) {
    if (!strcmp(comando, "mywait")) {
        printf("Executando o My Wait...\n");
        pid_t pid;
        while ((pid = waitpid(-1, NULL, WNOHANG)) > 0);

    } else { // clean&die
        printf("Executando o Clean & Die...\n");
        for (int i = 0; i < pgids_tam; i++) {
            pid_t *_pgid = retornaIndex(i);
            if (_pgid[0] != 0) {
                if (kill(-_pgid[1], SIGKILL) == -1)
                    perror("Falha ao matar um grupo");
            }
        }
        liberaVet();
        if (kill(-getpgrp(), SIGKILL) == -1)
            perror("Falha ao matar a shell");
    }
}
