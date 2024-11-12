
#ifndef HANDLERS_SER
#define HANDLERS_SER
#include <stdio.h>
#include"serv_cli_fifo.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

void hand_reviel(int sig){
    printf("server wake !, signal :%d",sig) ;
}

void fin_serveur(int sig, int fd1, int fd2){
    printf("server shutdown !, signal :%d",sig) ;
    close(fd1);
    close(fd2); 
    unlink(FIFO_1_PATH);
    unlink(FIFO_2_PATH);
    exit(0);
}
#endif