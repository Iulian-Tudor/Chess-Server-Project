#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#define READ 0
#define WRITE 1 
 
int main() { 

    bool exited = false;
    mkfifo("fisier.txt", 0666);

    pid_t pid = fork(); 
    if(pid > 0) { // parent

        char buffer[100];

        while(memcmp(buffer, "exit", 4) != 0){
            
            int desc = open("fisier.txt", O_WRONLY);
            while(desc == -1){
                desc = open("fisier.txt", O_WRONLY);
            }

            memset(buffer, '\0', 100);
            printf("Enter msg:\n");
            read(0, buffer, 100);

            write(desc, buffer, strlen(buffer));

            close(desc);

            sleep(3);

            desc = open("fisier.txt", O_RDONLY);
            while(desc == -1){
                desc = open("fisier.txt", O_RDONLY);
            }

            char buff[10];
            int len_read = read(desc, buff, 100);
            buff[len_read] = '\0';
            
            printf("Read from child: %s\n", buff);

            close(desc);

        }

        exited = true;
    } else { 
        // child
        while(!exited){
            sleep(1);

            int desc = open("fisier.txt", O_RDONLY);
            while(desc == -1){
                desc = open("fisier.txt", O_RDONLY);
            }

            char buff[10];
            int len_read = read(desc, buff, 100);
            buff[len_read] = '\0';
            close(desc);


            desc = open("fisier.txt", O_WRONLY);
            while(desc == -1){
                desc = open("fisier.txt", O_WRONLY);
            }
            
            printf("Read from parent: %s\n", buff);
            write(desc, "Am printat", 10);
            close(desc);
        }
    }
} 