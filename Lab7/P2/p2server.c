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

/* portul folosit */
#define PORT 2024
#define FILE_OPEN "file"
/* codul de eroare returnat de anumite apeluri */
extern int errno;

int main()
{
    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;
    char msgrasp[100] = " "; //mesaj de raspuns pentru client
    int sd;                  //descriptorul de socket

    
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[server]Eroare la socket().\n");
        return errno;
    }

    
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);


    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    
    if (listen(sd, 5) == -1) {
        perror("[server]Eroare la listen().\n");
        return errno;
    }

    int i = 0;

    char notFound[4096];
    bzero (notFound, 4096);

    strcat(notFound, "Unknown command");
    while (1)
    {
        i++;
        int client;
        int length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        client = accept(sd, (struct sockaddr *)&from, &length);

        if (client < 0) {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        int pid = fork();

        if (pid == 0) {
            
            write(client, &i, 4);

            FILE *file = fopen(FILE_OPEN, "r");

            char line[4096];

            char *p;
            int found = 0;
            while ( fgets(line, sizeof(line), file) != NULL ) {
                line[(int)strlen(line) - 2] = '\0';

                int lineLength = strlen(line);

                write(client, &lineLength, 4);
                write(client, line, lineLength);

            }
            
            int stop = -1;
            write(client, &stop, 4);

            printf("Download finished for client %d\n", i);
            
            int finishedSucces = 1;
            // write(sd, &finishedSucces, 4);

            return 0;
        }

        
    }
}
