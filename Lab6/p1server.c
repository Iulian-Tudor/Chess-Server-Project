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
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

/* portul folosit */
#define PORT 2024

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
    
    int nr;
    while (1)
    {
        int client;
        int length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        client = accept(sd, (struct sockaddr *)&from, &length);

        if (client < 0) {
            perror("[server]Eroare la accept().\n");
            continue;
        }
       
        printf("[server]Asteptam mesajul...\n");
        fflush(stdout);

        if (read(client, &nr, 4) <= 0) {
            perror("[server]Eroare la read() de la client.\n");
            close(client); 
            continue;
        }

        
        nr = nr + 1;
        
        printf("[server]Mesajul a fost receptionat...%d cu numarul de ordine %d\n", nr, i);
       
        write(client, &nr, 4);
        write(client, &i, 4);

        i++;
    }
}
