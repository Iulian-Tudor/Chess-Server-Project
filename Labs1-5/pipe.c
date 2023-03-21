#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h>
#include <stdbool.h>
 
#define READ 0
#define WRITE 1 

int main()
{   
    int fd[2];
    pid_t pid_fiu;
    int nr;

    if (pipe(fd)==-1){
            printf("Pipe error");
            return 1;
        }


    if (pid_fiu=fork()==-1){
        printf("Eroare la fork \n");
        return 2;
    }


    if (pid_fiu!=0){//parinte

        close(fd[0]);
        int x = 25;
        if (write(fd[1], &x, sizeof(x))==-1){
            printf("write error \n");
            return 3;
        }
        close(fd[1]);

    } else {//copil
        close(fd[1]);
        
        int y;
        if (read(fd[0], &y, sizeof(y))==-1){
            printf("read error \n");
            return 4;
        }

        printf("%d\n", y);
        close(fd[0]);
    
    }

    return 0;
}