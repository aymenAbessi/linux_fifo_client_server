#include "serv_cli_fifo.h"
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/file.h> 

int fd1, fd2, n, server_pid;

// sync between fifos
int lock_fd1; 
int lock_fd2; 

//help to keep the client alive until he read the response 
volatile sig_atomic_t response_received = 0;

void recive_server_r(int sig) {
    printf("PID: %d - The server has answered your question! Signal: %d\n", getpid(), sig);

    // try to get the lock before reading
    printf("PID: %d - Trying to acquire lock to read...\n", getpid());
    if (flock(lock_fd2, LOCK_EX) == -1) {
        perror("Failed to acquire lock");
        exit(1);
    }
    printf("PID: %d - Lock acquired for reading.\n", getpid());

    Response server_r;
    // read the response from the server
    if (read(fd2, &server_r, sizeof(server_r)) != sizeof(server_r)) {
        perror("Failed to read from FIFO 2!");
        exit(6);
    }
    printf("PID: %d - The response is:\n", getpid());
    for (int i = 0; i < n; i++) {
        printf("PID: %d - Number %d: %d\n", getpid(), i, server_r.random_numbers[i]);
    }

    // release the lock after reading
    if (flock(lock_fd2, LOCK_UN) == -1) {
        perror("Failed to release lock");
        exit(1);
    }
    printf("PID: %d - Lock released after reading.\n", getpid());

    // inform server that the response is read
    kill(server_r.pid_s, SIGUSR1);
    response_received = 1;
}

int main() {
   
    srand(time(NULL));

    // read the server PID
    FILE *pid_file = fopen("server_pid.txt", "r");
    if (pid_file == NULL) {
        perror("Failed to open server PID file");
        exit(1);
    }
    fscanf(pid_file, "%d", &server_pid);
    fclose(pid_file);

    // open FIFO 1 for writing and FIFO 2 for reading
    fd1 = open(FIFO_1_PATH, O_WRONLY); 
    fd2 = open(FIFO_2_PATH, O_RDONLY); 
    if ((fd1 == -1 || fd2 == -1)) {
        perror("Error opening the pipes");
        exit(6);
    }
    lock_fd1 = fd1; 
    lock_fd2 = fd2;

    // attach signal handler for receiving server response
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = recive_server_r;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("Failed to set signal handler for SIGUSR1");
        exit(5);
    }

    // prepare question to send
    Question client_q;
    client_q.n = (rand() % NMAX) + 1;
    client_q.pid_c = getpid();
    n = client_q.n;

    // try to get the lock before writing
    printf("PID: %d - Trying to acquire lock to write...\n", getpid());
    if (flock(lock_fd1, LOCK_EX) == -1) {
        perror("Failed to acquire lock");
        exit(1);
    }
    printf("PID: %d - Lock acquired for writing.\n", getpid());

    if (write(fd1, &client_q, sizeof(client_q)) != sizeof(client_q)) {
        perror("Failed to write the question");
        exit(7);
    }

    // release the lock after writing
    if (flock(lock_fd1, LOCK_UN) == -1) {
        perror("Failed to release lock");
        exit(1);
    }
    printf("PID: %d - Lock released after writing.\n", getpid());

    // inform the server that a question has been sent
    kill(server_pid, SIGUSR2);

    // wait for response signals
    while (!response_received) {
        pause();
    }
    printf("PID: %d - Client process ending.\n", getpid());

    close(fd1);
    close(fd2);

    exit(0);
}
