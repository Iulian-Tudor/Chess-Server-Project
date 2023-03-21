#include "header.h"


int s_listen()
{

struct addrinfo hostinfo, *res;

int sock_fd;

int server_fd; //  fd pentru listen
int a;


memset(&hostinfo, 0, sizeof(hostinfo)); //alocare de memorie

hostinfo.ai_family = AF_UNSPEC;  
hostinfo.ai_socktype = SOCK_STREAM;
hostinfo.ai_flags = AI_PASSIVE;  

getaddrinfo(NULL, PORT, &hostinfo, &res); // se incarca adresele


    server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(server_fd < 0)
    {
      perror("Eroare la socket()\n");
      return errno;
    }


    a = bind(server_fd, res->ai_addr, res->ai_addrlen);
    if(a != 0)
    {
        perror("Eroare la bind \n");
        return -1 ;
    }

    a = listen(server_fd, waitingList);
    if(a < 0)
    {
      perror("Eroare la listen()\n");
      return errno;
    }


return server_fd;

}

int conn_server(int server_fd)
{
    char ipstr[INET_ADDRSTRLEN];
    int port;


    int new_sd;
    struct sockaddr_storage remote_info ;
    socklen_t addr_size;

    addr_size = sizeof(addr_size);
    new_sd = accept(server_fd, (struct sockaddr *) &remote_info, &addr_size);
    if (new_sd < 0)
    {
      perror("Eroare la accept()\n");
      return errno;
    }
    return new_sd;

}


int main()
{

    board.resize(8);
    for (int i = 0; i < 8; i++)
        board[i].resize(8);

     startPhase(); // Umplem tabela cu piese

    cout << "Serverul este on"  << endl ;

    int server_fd = s_listen() ;
    if (server_fd == -1)
    {
        cout << "Eroare!" ;
        return 1 ;
    }


    string whitePlayer="\nDumneavoastra jucati cu ALBELE. (Piesele din stanga) \n";
    string blackPLayer="\nDumneavoastra jucati cu Negrele. (Piesele din dreapta) \n";

    string options = "\nAveti la dispozitie urmatoarele comenzi: \n'print' - printeaza tabla de joc \n'quit' - va deconecteaza de la joc \n'reset' - reseteaza jocul \nMiscarile se pot realiza sub forma de coordonate ale tablei 'D2D4'.\n";

    pthread_t threads[maxFD]; 

    FD_ZERO(&stare);

    while(1) 
    {
        int readFD;
        void *arg;

        readFD = conn_server(server_fd);

        if (readFD >=0)
        {
            write_to_client(readFD,"\nClient conectat, bine ati venit, aveti descriptorul: ");
            write_to_client(readFD, to_string(readFD));
            if (readFD > maxFD)
            {
                write_to_client(readFD, "\nPrea multi clienti sunt deja in asteptare. Incercati mai tarziu.\n");
                close(readFD);
                continue;
            }

            pthread_mutex_lock(&mlock);  

            FD_SET(readFD, &stare);  
            pthread_mutex_unlock(&mlock); 
 
            pthread_create(&threads[readFD], NULL, read_from_client, (void *) (intptr_t) readFD);
        }
        if(readFD<6 && readFD==4)
        {
            write_to_client(readFD, options);
            write_to_client(readFD, whitePlayer);
        }
        else if(readFD<6 && readFD==5)
        {
            write_to_client(readFD, options);
            write_to_client(readFD, blackPLayer);
        }
        else 
        {
            write_to_client(readFD, "\nUn joc este deja in desfasurare. Va rugam sa asteptati finalizarea lui.\n");
        }
     
    }
    return 0;
}
