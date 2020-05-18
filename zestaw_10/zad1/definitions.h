#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>

#define NONE 0
#define PLAYER_SEARCH 1
#define GAME_STARTED 2
#define BAD_USERNAME 4
#define GAME_REFRESH 5
#define GAME_REFRESH_MOVE 6

#define MAX_MSG_LEN 4096
#define SERWER_PORT 8888
#define SERWER_IP "127.0.0.1"
#define MAX_CONNECTION 10
#define MAX_PLAYERS 16
#define PL1_CROSS 1
#define PL2_CROSS 0
#define PL1 1
#define PL2 2

#define new(Type) calloc(1, sizeof(Type))
#define New(Type, Size) calloc(Size, sizeof(Type))

typedef struct {
    char board[9];
    int player1;
    int player2;
    int mode;
    int finish;
    int tour;
} Game;

typedef struct {
    char * username;
    int socket;
    int is_in_game;
} client;

typedef struct sockaddr_in sockaddr_in;

typedef struct {
    sockaddr_in address;
    int connection_socket;
    int port;
    int max_mess_len;
    int max_conn;
    char ip[64];
} Connection;