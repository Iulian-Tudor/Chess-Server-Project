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

    char key[50];
    bzero (key, 50);

    printf("[client]Introduce the key: ");
    scanf("%s", key);

    if (write(sd, key, 50) <= 0) {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
    }

    char line[4096];
    bzero (line, 4096);

    int nrLines;

    read(sd, &nrLines, 4);

    for (int i = 0; i < nrLines; i++) {
        read(sd, line, 4096);
        printf("%s\n", line);
    }


    close(sd);
}
