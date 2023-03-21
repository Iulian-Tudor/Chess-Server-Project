#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h>
#include <stdbool.h>
 
#define READ 0
#define WRITE 1 
 
int main() { 
    int pipe_fds1[2];  
    pipe(pipe_fds1);

    int pipe_fds2[2];  
    pipe(pipe_fds2);

    dup2(1, pipe_fds1[READ]);
 
    pid_t pid = fork(); 
    if(pid > 0) { // parent

        sleep(1);

        close(pipe_fds2[WRITE]);

        char buff[10];
        int len_read = read(pipe_fds2[READ], buff, 100);
        buff[len_read] = '\0';

        printf("Read from child: %s\n", buff);
    } else { 
        // child
        
        execlp("cut", "cut", "-d:", "-f1,3-5", NULL);
    }
} 

