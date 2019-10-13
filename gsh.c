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

#define TAM_PGIDS 200
extern pid_t pgids[TAM_PGIDS];

void imprimePrompt(){
    printf("\nghs>");
}

Lista* leLinha(){
    Lista* comandos = NULL;
    char linha[500];
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
    char* split = strtok(comando, " ");
    while(split != NULL){
        args[i++] = split;
        split = strtok(NULL, " ");
    }
    args[i] = NULL;
    pid_t pid;
    if((pid = fork()) < 0) //como tratar todos os sinais que um processo recebe para matar todo o grupo
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
        srand(time(NULL)); //pega o horário como semente da função rand()
        int moeda = rand() % 2; //os processos estão com a mesma moeda
        printf("moeda: %d\n", moeda);
        if(moeda) { //moeda pode ser 0 ou 1, se moeda == 1 cria ghost, caso contrario, nao cria
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
    printf("\nFoi Capturado um Ctrl+C (SIGINT)\n");
    if(existeGrupo()) {
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
}

void trata_SIGTSTP(int signum){ //parar somente os descentes da shell, ela nao
    printf("\nFoi Capturado um Ctrl+Z (SIGTSTP)\n");
    int i;
    for(i = 0; i < TAM_PGIDS; i++) {
        if(pgids[i] != 0) {
            kill(-pgids[i], SIGTSTP);
        }
    }

}

int existeGrupo(){
    int i;
    for(i = 0; i < TAM_PGIDS; i++) {
        if(pgids[i] != 0) {
            return 1;
        }
    }
    return 0;
    
//    int count = 0;
//    for(int i = 0; TAM_PGIDS>i; i++){
//        if(pgids[i] == 0)
//            count++;
//    }
//    count == TAM_PGIDS ? 1 : 0;
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
        for(i = 0; i < TAM_PGIDS; i++) {
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
