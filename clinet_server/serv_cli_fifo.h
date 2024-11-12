
#ifndef SERV_CLIENT_FIFO 
#define SERV_CLIENT_FIFO

#include <fcntl.h>


#define NMAX 10 
#define FIFO_1_PATH "/tmp/question"
#define FIFO_2_PATH "/tmp/response"




typedef struct Question
{
 pid_t pid_c ; 
 int n ;
}Question;

typedef struct Response
{
 pid_t pid_s ; 
 int random_numbers[NMAX] ;
}Response ;

#endif