#include "Handlers_Serv.h"
#include "serv_cli_fifo.h"
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


int pid;

// to pass to fin_serveur to close descripteurs des fichiers
struct server_fds {
    int fd1;
    int fd2;
};

struct server_fds fds;



void generateNumbers(int n, int *numbers) {
        printf("the answer is :\n");
    for (int i = 0; i < n; i++) {
        numbers[i] = rand() % 100;
         printf("%d\n", numbers[i]);
    }
    fflush(stdout);
}

void recive_q_handler(int sig) {

        printf("a client send a question to be answer !, signale :%d\n", sig);
        // read client question
        Question client_q;
        if (read(fds.fd1, &client_q, sizeof(client_q)) != sizeof(client_q)) {
            perror("failed to read from fifo1");
            exit(3);
        }
        // write the answer and notify the clinet by sigusr2
        Response server_r;
        printf("answer for client %d\n",client_q.pid_c);
        generateNumbers(client_q.n, server_r.random_numbers);
        server_r.pid_s = pid;
        if (write(fds.fd2, &server_r, sizeof(server_r)) != sizeof(server_r)) {
            perror("failed to write to fifo2");
            exit(4);
        }
    
      
        //inform client that the question is answered 
        kill(client_q.pid_c, SIGUSR1);
       
}

void fin_server_handler(int sig) {
    fin_serveur(sig, fds.fd1, fds.fd2);
}

void  answer_readed_handler(){
    printf("the client read the answer !\n");
}
int main() {
    pid = getpid();

    // make the pid access to clients
    FILE *pid_file = fopen("server_pid.txt", "w");
    if (pid_file == NULL) {
        perror("Failed to open pid file");
        exit(1);
    }
    fprintf(pid_file, "%d\n", pid);
    fclose(pid_file);

    // create the two pipes
    if (mkfifo(FIFO_1_PATH, 0666) != 0 || mkfifo(FIFO_2_PATH, 0666) != 0) {
        perror("failed to create the two pipe");
        exit(1);
    }
  

    // open fifo 1 en lecture and fifo 2 en ecrtiture
    fds.fd1 = open(FIFO_1_PATH, O_RDONLY); 
    fds.fd2 = open(FIFO_2_PATH, O_WRONLY); 
    if (fds.fd1 == -1 || fds.fd2 == -1) {
        perror("failed to open the two pipe");
        exit(2);
    }

    // attach the handler for SIGUSR2 toknow if the client write a question
    struct sigaction sa;
    sa.sa_handler = recive_q_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("Failed to set signal handler");
        exit(5);
    }

    signal(SIGINT, fin_server_handler);
    signal(SIGUSR1, answer_readed_handler);

    // listen for signals
    while (1) {
        pause();
    }

    return 0;
}
