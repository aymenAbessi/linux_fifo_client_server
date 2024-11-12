#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_CLIENTS 6 

int main() {
    srand(time(NULL)); 

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pid_t pid = fork();
        int delay = rand() % 7; 
        if (pid == 0) { 
            // each client waits a random amount of time before executing
            sleep(delay);
            execl("./client", "client", NULL);
            perror("execl failed");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
    }

    

    printf("All clients have finished.\n");

   exit(0);
}
