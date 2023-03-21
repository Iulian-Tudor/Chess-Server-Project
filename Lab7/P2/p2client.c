#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

extern int errno;

int port;

int main(int argc, char *argv[])
{
    int sd;                   
    struct sockaddr_in server; // structura folosita pentru conectare


    if (argc != 3) {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Eroare la socket().\n");
        return errno;
    }


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);


    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("[client]Eroare la connect().\n");
        return errno;
    }

    char fileName[50];
    int clientCount;
    read(sd, &clientCount, 4);

    strcpy(fileName, "");

    sprintf(fileName, "downloaded_%d.txt", clientCount);

    FILE* file = fopen(fileName, "a");

    int bytesRead;

    while (1) {
        read(sd, &bytesRead, 4);

        if (bytesRead == -1) {
            break;
        }

        char line[4096];

        read(sd, line, bytesRead);
    
        char prLine[4096];
        strcpy(prLine, "");
        strncat(prLine, line, bytesRead);
        // printf("BR: %d\n", bytesRead);

        fprintf(file, "%s\n", prLine);

    }
    fclose(file);

    // int status;
    // read(sd, &status, 4);

    // if (status = 1) {
    //     printf("Successfully downloaded: %d\n", clientCount);
    // } else {
    //     printf("Download error: %d\n", clientCount);
    // }  

    close(sd);
}
