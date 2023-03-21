#ifndef HEADER_
#define HEADER_

#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <iostream>
#include <cstring> 	
#include <arpa/inet.h> 	
#include <netdb.h>
#include <sys/socket.h>
#include <pthread.h>
#include <vector>
#include <list>
#include <iterator>
#include <sstream>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
using namespace std;

#define white_king  "\u2654" // ♔
#define white_q     "\u2655" // ♕
#define white_r     "\u2656" // ♖
#define white_b     "\u2657" // ♗
#define white_k     "\u2658" // ♘
#define white_p     "\u2659" // ♙
#define black_king  "\u265A" // ♚
#define black_q     "\u265B" // ♛
#define black_r     "\u265C" // ♜
#define black_b     "\u265D" // ♝
#define black_k     "\u265E" // ♞
#define black_p     "\u265F" // ♟
#define blackSquare "\u25A0"
#define whiteSquare "\u25A1"

const char* PORT = "20000";
const int maxlength = 1024;
const int maxFD = 10; 
const int waitingList = 5;
extern int errno;

const string culoare_text ("\033[0;93m");
const string culoare_coordonate ("\033[0;91m");
const string culoare_neagra("\033[0;30m");
const string culoare_alba("\033[0;97m");
int pasi=0;

fd_set stare;

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER; // pentru a evita race condition

pthread_mutex_t mutex_table = PTHREAD_MUTEX_INITIALIZER; // pentru array

vector<vector<string> >board;
list<string> captures;
list<string>::iterator nr_capturi;


int init_a(char * move)
{
    int a;
    a = move[0]-65;
    return a;
}

int init_b(char * move)
{
    int b;
    b = move[1]-49;
    return b;
}

int init_c(char * move)
{
    int c;
    c = move[2]-65;
    return c;
}

int init_d(char * move)
{
    int d;
    d = move[3]-49;
    return d;
}

int write_to_client(int fd, string date)
{   

    int memorie;
    memorie = send(fd, date.c_str(), strlen(date.c_str()),0);

    return 0;
}



void startPhase()
{

    pthread_mutex_lock(&mutex_table);

    captures.clear();

    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            board[i][j] = "-"; // consideram - ca fiind spatiu gol.
        }
    }


 //Piese albe
    for (int i=0; i<8; i++)
    {
        board[1][i] = white_p;
    }
    board[0][0]= white_r;
    board[0][7]= white_r;
    board[0][1]= white_k;
    board[0][6]= white_k;
    board[0][2]= white_b;
    board[0][5]= white_b;
    board[0][3]= white_q;
    board[0][4]= white_king;


 //Piese Negre
    for (int i=0; i<8; i++)
    {
        board[6][i] = black_p;
    }
    board[7][0]= black_r;
    board[7][7]= black_r;
    board[7][1]= black_k;
    board[7][6]= black_k;
    board[7][2]= black_b;
    board[7][5]= black_b;
    board[7][3]= black_q;
    board[7][4]= black_king;

    pthread_mutex_unlock(&mutex_table);

}

void showTable(int client) // va afisa in terminalul fiecarui client o tabla de joc
{
    char coordonate_numerice;

    string coordonate= culoare_coordonate + "  A  B  C  D  E  F  G  H\n";
    write_to_client(client, "\n" + coordonate);

    for (int j=7; j>=0; j--)
    {
        coordonate_numerice= j + 49; // int to char
        string coordonate_numerice_str="";
        coordonate_numerice_str += coordonate_numerice; //char to string
        write_to_client(client, coordonate_numerice_str); 

        for (int i=0; i<8; i++)
        {
            if (board[i][j]=="-")
            {
                if (((j % 2 == 0) && (i % 2 == 0)) || ((j % 2 == 1) && (i % 2 == 1))) // verifica daca patratul este alb
                {
                    board[i][j]=whiteSquare;
                }
                else
                    board[i][j]=blackSquare;

            }

            write_to_client(client, " " + board[i][j] + " ");
        }

        write_to_client(client, culoare_coordonate + coordonate_numerice_str + "\n");
        
    }

    write_to_client(client, coordonate + "\n\n");

    string piese_capturate="Piese capturate: ";
    for (nr_capturi = captures.begin(); nr_capturi!= captures.end(); ++nr_capturi)
        piese_capturate = piese_capturate + *nr_capturi;

    write_to_client(client, piese_capturate + culoare_text + "\n");

}

void resetTable()
{
    startPhase();
    pasi=0;
    for (int writeFD =4; writeFD<maxFD; writeFD++)
        {
            if(FD_ISSET(writeFD, &stare))
            {
                showTable(writeFD);
            }
        }

}

int quit(int client)
{
    write_to_client(client, "Ati fost deconectat.\n");
    close(client);
    return 0;
}



void checkWinner(int client)
{
    bool white_king_alive=false;
    bool black_king_alive=false;

    for (int writeFD =4; writeFD<maxFD; writeFD++)
    {
        if (FD_ISSET(writeFD, &stare))
        {
            showTable(writeFD);
            for (int i=0; i<8; i++)
            {
                for (int j=0; j<8; j++)
                {
                    if(board[i][j]==white_king)
                        white_king_alive=true;
                    else if (board[i][j]==black_king)
                        black_king_alive=true;
                }
            }

            if (white_king_alive==true && black_king_alive==false)
            {
                write_to_client(client, "Felicitari, ati castigat cu Alb!");
                close(client);
            }
            else if (black_king_alive==true && white_king_alive==false)
            {
                write_to_client(client, "Felicitari, ati castigat cu Negru!");
                close(client);
            }
        
        }
    }
}


void step(char * move)
{
    bool realizare = false;

    int a,b,c,d ;

    a=init_a(move);
    b=init_b(move);
    c=init_c(move);
    d=init_d(move);

    if (!(board[a][b] == blackSquare) && !(board[a][b] == whiteSquare))
    {
        realizare = true;
        pthread_mutex_lock(&mutex_table);

        if (board[c][d] != "-" &&
            board[c][d] != blackSquare &&
            board[c][d] != whiteSquare)
            {
                captures.insert(captures.begin(), board[c][d]);
            }
        
        board[c][d]=board[a][b];
        board[a][b]="-";
        pthread_mutex_unlock(&mutex_table);
        
    }

}

bool valid_queen(char * move)
{
    bool realizare = false;


    int a, b , c ,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if (b == d || a == c || abs(d - b) == abs(c - a)) realizare=true; 
    return realizare;

}

bool valid_king(char * move)
{
    bool realizare = false;


    int a,b,c,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if (abs(d-b)<=1 && abs(c-a)<=1) realizare=true; 
    return realizare;

}

bool valid_knight(char * move)
{
    bool realizare = false;


    int a,b,c,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if (abs(d-b)==2 && abs(c-a)==1 || abs(d-b)==1 && abs(c-a)==2) realizare=true; 
    return realizare;

}

bool valid_bishop(char * move)
{
    bool realizare = false;


    int a,b,c,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if (abs(d-b)==abs(c-a)) realizare=true;

    return realizare;

}

bool valid_rook(char * move)
{
    bool realizare = false;


    int a,b,c,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if (b==d || a==c) realizare=true;
    return realizare;

}

bool valid_pawn(char * move)
{
    bool realizare = false;


    int a,b,c,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if (a==c) realizare=false;

    if (d=b && c==a+1 || d==b && c==a-1) realizare = true;

    if ((a==1 && d==b && c==a+2) || (a=6 && d==b && c==a-2)) realizare=true;

    if (abs(d-a)==1 && c==a+1 || abs(d-b)==1 && c==a-1) realizare=true;

    return realizare;

}


bool valid_step(char * move)
{
    bool realizare = true;


    int a,b,c,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if ((board[a][b] == blackSquare) && (board[a][b] == whiteSquare))
    {
        realizare=false;
    } 
    else 
    {
        pthread_mutex_lock(&mutex_table);  

        if (board[c][d] != "-" && board[c][d] != blackSquare  && board[c][d] != whiteSquare)
            realizare=true;

        pthread_mutex_unlock(&mutex_table); 
    }

    return realizare;
}

bool valid_white(char * move)
{
    bool realizare = true;


    int a,b,c,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if(board[a][b]==white_b && valid_bishop(move)||
            board[a][b]==white_k && valid_knight(move)||
            board[a][b]==white_p && valid_pawn(move)||
            board[a][b]==white_q && valid_queen(move)||
            board[a][b]==white_r && valid_rook(move)||
            board[a][b]==white_king && valid_king(move))
            {
                realizare=true;
            }
            else realizare=false;
   
    return realizare;
}

bool valid_black(char * move)
{
    bool realizare = true;


    int a,b,c,d ;

    a=init_a(move); //coloana
    b=init_b(move); //linie
    c=init_c(move); //coloana
    d=init_d(move); //linie

    if(board[a][b]==black_b && valid_bishop(move) ||
            board[a][b]==black_k && valid_knight(move)||
            board[a][b]==black_p && valid_pawn(move)||
            board[a][b]==black_q && valid_queen(move)||
            board[a][b]==black_r && valid_rook(move)||
            board[a][b]==black_king && valid_king(move))
            {
                realizare=true;
            }
            else realizare=false;


    return realizare;
}


void jucator(char * input, int client)
{
    char p, q, r; 

    if (input[0] == 'p' || input[0]=='P') showTable(client);

    if (input[0] == 'r' || input[0]=='R') resetTable();

    if (input[0] == 'q' || input[0]=='Q')  quit(client);

    if (input[0]>='A' &&
        input[0]<='H' &&
        input[1]>='1' &&
        input[1]<='8' &&
        input[2]>='A' &&
        input[2]<='H' &&
        input[3]>='1' &&
        input[3]<='8')
        {
            if(client==4)
            {
                if (valid_step(input) && valid_white(input) && pasi%2==0) 
                { 
                    step(input);
                    pasi++;
                    checkWinner(client);
                    write_to_client(client, "\nMutare: ");
                    write_to_client(client, input);
                    write_to_client(client, "\n");
                    showTable;

                }
                else 
                {
                    if (valid_step(input) && pasi%2==1)
                        write_to_client(client, "Este randul oponentului de a muta. \n");
                
                    if (valid_step(input)==false && pasi%2==0)
                        write_to_client(client, "Miscarea nu corespunde regulamentului. \n");

                    if (valid_step(input) && valid_white(input)==false && pasi%2==0)
                        write_to_client(client, "Nu puteti muta piesa oponentului. \n");

                }
            } 
            else if(client==5)
            {
                if (valid_step(input) && valid_black && pasi%2==1)
                {
                    step(input);
                    pasi++;
                    checkWinner(client);
                    write_to_client(client, "\nMutare: ");
                    write_to_client(client, input);
                    write_to_client(client, "\n");
                    showTable;

                }
                else 
                {
                    if (valid_step(input) && pasi%2==0)
                        write_to_client(client, "Este randul oponentului de a muta. \n");
                
                    if (valid_step(input)==false && pasi%2==1)
                        write_to_client(client, "Miscarea nu corespunde regulamentului. \n");

                    if (valid_step(input) && valid_black(input)==false && pasi%2==1)
                        write_to_client(client, "Nu puteti muta piesa oponentului. \n");
                }
            }



        }
        
}

void *read_from_client(void *argument)
{
    int readFD;
    int writeFD;

    char mesaj[maxlength];
    int mesajLength;

    readFD = static_cast<int>(reinterpret_cast<intptr_t>(argument));
    while (1)
    {
        mesajLength=read(readFD, mesaj, sizeof(mesaj));
        if (mesajLength <1)
        {
            cout << "S-a deconectat clientul, deci se elibereaza descriptorul. " << readFD << endl;
            pthread_mutex_lock(&mlock);
            FD_CLR(readFD, &stare);
            pthread_mutex_unlock(&mlock);
            close(readFD);
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&mlock);

        for (writeFD=4; writeFD<maxFD; writeFD++)
        {
            if (FD_ISSET(writeFD, &stare) && (readFD!=writeFD))
            {
                write_to_client(writeFD, mesaj);
            }
        }

        pthread_mutex_unlock(&mlock);

        if((readFD<6) && (readFD>3))
        {
            jucator(mesaj,readFD);
        }
        else write_to_client(readFD,"Ne pare rau. Un meci este deja in desfasurare. Va rugam sa asteptati.");
    }   

    return NULL;
}

#endif
