#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // strlen()
#include <sys/socket.h> // socket()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_pton()
#include <pthread.h>
#include <time.h>

#define NONE 0
#define PLAYER_SEARCH 1
#define GAME_STARTED 2
#define BAD_USERNAME 4

#define SERWER_PORT 8888
#define MAX_MSG_LEN 4096
#define SERWER_IP "127.0.0.1"

const short HTTP_PORT = 80;

#define new(Type) calloc(1, sizeof(Type))
#define New(Type, Size) calloc(Size, sizeof(Type))

void create_and_run_thread(void * (*f) (void *), void * args) {
    pthread_t * thread = new (pthread_t);
    pthread_create(thread, NULL, f, args);
}

typedef struct sockaddr_in sockaddr_in;

char * String(char * s) {
    char * str = New(char, MAX_MSG_LEN);
    strcpy(str, s);
    return str;
} 

char * con_receive(int socket) {
    char * message = New (char, MAX_MSG_LEN);
    recv(socket, message, MAX_MSG_LEN, 0);
    return message;
}

void con_send(int socket, char * message) {
    send( socket, message, strlen(message), 0 );
}

int connect_to_server(int port, char * ip) {
    sockaddr_in * address = new (sockaddr_in);
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    inet_pton(AF_INET, ip, &(address->sin_addr));
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    connect(server_socket, (struct sockaddr *) address, sizeof(*address));
    return server_socket;
}

char * board;

char * parse_response(char * msg) {
    char * key;
    char * data;
    key = strtok(msg, "|");
    if (!strcmp(key, "map")) {
        board = strtok(NULL, "|");
    }
    else return key;
    
}

int state = NONE;

void * f(void * sockt) {
    int sckt = *((int *) sockt);
    while (1) {
        char * response = con_receive(sckt);
        if (!strcmp(parse_response(response), "search")) {
            state = PLAYER_SEARCH;
        }
        if (!strcmp(parse_response(response), "start")) {
            state = GAME_STARTED;
        }
        if (!strcmp(parse_response(response), "badusr")) {
            state = BAD_USERNAME;
        }  
        //printf("odp: %s\n", response);
        //sleep(1);
    }

}

int main(int argc, char ** argv) {
    char * username = argv[1];
    char * ip_server_address = argv[3];
    int port_num = atoi(argv[4]);
    board = New (char, 9);
    for (int i = 0; i < 9; ++i) board[i] = ' ';
    char buffer[128];
    int server_socket = connect_to_server(port_num, ip_server_address);
    con_send(server_socket, username);
    create_and_run_thread(f, &server_socket);
    while(1) {
        int old_state = state;
        switch (state) {
            case NONE:
                break;
            case PLAYER_SEARCH:
                printf("waiting for a player to pair\n");
                break;
            case GAME_STARTED:
                printf("starting a game!\n");
                printf("%c|%c|%c\n- - -\n%c|%c|%c\n- - -\n%c|%c|%c\n", board[0], board[1], board[2], board[3], board[4], board[5], board[6], board[7], board[8]);
                break;  
            case BAD_USERNAME:
                printf("this username is not free.\nStopping..\n");
                exit(0);
                break;
            default:
                break;
        }
        while (old_state == state);
            //sleep(1);

    }

    shutdown( server_socket, SHUT_RDWR);
   
    return 0;
}