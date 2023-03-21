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

    bool exited = false;

    int pipe_fds1[2];  
    pipe(pipe_fds1);

    int pipe_fds2[2];  
    pipe(pipe_fds2);
 
    pid_t pid = fork(); 
    if(pid > 0) { // parent

        char buffer[100];

        while(memcmp(buffer, "exit", 4) != 0){
            memset(&buffer, '\0', 100);
            printf("Introduce-ti mesaj:\n");
            read(1, &buffer, 100);

            close(pipe_fds1[READ]);
            write(pipe_fds1[WRITE], buffer, strlen(buffer));

            sleep(3);

            close(pipe_fds2[WRITE]);
    
            char buff[10];
            int len_read = read(pipe_fds2[READ], buff, 100);
            buff[len_read] = '\0';
    
            printf("Read from child: %s\n", buff);

        }

        exited = true;
    } else { 
        // child
        while(!exited){
            close(pipe_fds1[WRITE]);
    
            char buff[10];
            int len_read = read(pipe_fds1[READ], buff, 100);
            buff[len_read] = '\0';
    
            printf("Read from parent: %s\n", buff);

            close(pipe_fds2[READ]);
            write(pipe_fds2[WRITE], "Am printat", 10);
        }
    }
} 
