#include <stdio.h>
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
#include <sys/epoll.h>

#define MAX_MSG_LEN 4096
#define SERWER_PORT 8888
#define SERWER_IP "127.0.0.1"
#define MAX_CONNECTION 10
#define MAX_PLAYERS 16
#define PL1_CROSS 1
#define PL2_CROSS 0

#define new(Type) calloc(1, sizeof(Type))
#define New(Type, Size) calloc(Size, sizeof(Type))

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct sockaddr_in sockaddr_in;

typedef struct {
    char * username;
    int socket;
    int is_in_game;
} client;

client players[MAX_PLAYERS];

typedef struct {
    char board[9];
    int player1;
    int player2;
    int mode;
    int finish;
} Game;

Game games[MAX_PLAYERS];

void create_and_run_thread(void * (*f) (void *), void * args) {
    pthread_t * thread = new (pthread_t);
    pthread_create(thread, NULL, f, args);
}


typedef struct {
    sockaddr_in address;
    int connection_socket;
    int port;
    int max_mess_len;
    int max_conn;
    char ip[64];
} Connection;

char * String(char * s) {
    char * str = New(char, MAX_MSG_LEN);
    strcpy(str, s);
    return str;
} 

struct epoll_event event, events[MAX_PLAYERS];
int  epoll_fd;

Connection * new_connection(int port, int max_mess_len, char * ip, int max_conn) {
    Connection * conn = new(Connection);
    conn->address.sin_family = AF_INET;
    conn->address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &conn->address.sin_addr);
    conn->connection_socket = socket(AF_INET, SOCK_STREAM, 0);

    conn->max_conn = max_conn;
    bind(conn->connection_socket, (struct sockaddr_in *) &(conn->address), sizeof(conn->address));
    return conn;
}

void con_listen(Connection * conn) {
    listen(conn->connection_socket, conn->max_conn);
}

sockaddr_in * new_address() {
    return NULL;
}

int con_accept(Connection * conn, sockaddr_in * client) {
    int len;
    int a = accept(conn->connection_socket, (sockaddr_in *) client, &len);
    return a;
}

char * con_receive(int socket) {
    char * message = New (char, MAX_MSG_LEN);
    if (recv(socket, message, MAX_MSG_LEN, 0) == -1)
        return NULL;
    return message;
}

void con_send(int socket, char * message) {
    send( socket, message, strlen(message), 0 );
}

int save_username_at(char * str, int client) {
    for (int i = 1; i < MAX_PLAYERS; ++i) {
        if (players[i].socket == 0) {
            pthread_mutex_lock(&mutex);
            players[i].socket = client;
            players[i].username = String(str);
            pthread_mutex_unlock(&mutex);
            return i;   
        }
    }
    return -1;
}

void * search_for_player(void * cl) {
    int client = *((int *) cl);
    int place = -1;
    con_send(client, String("search"));
    
    for (int i = 1; i < MAX_PLAYERS; ++i) if (client == players[i].socket) place = i;
        int wait = 1;
        while (wait) {
            for (int i = 1; i < MAX_PLAYERS && wait; ++i) {
                pthread_mutex_lock(&mutex);
                if (!players[i].is_in_game && i != place && players[i].socket != 0) {
                    //printf("%d %d %d", players[i].is_in_game, )
                    players[i].is_in_game = 1;
                    players[place].is_in_game = 1;
                    for (int j = 1; j < MAX_PLAYERS; ++j) {
                        if (games[j].player1 == 0) {
                            games[j].board[0] = games[j].board[1] = games[j].board[2] = games[j].board[3] = games[j].board[4] = games[j].board[5] = games[j].board[6] = games[j].board[7] = games[j].board[8] = ' '; 
                            games[j].player1 = place;
                            games[j].player2 = i;
                            games[j].mode = PL1_CROSS;
                            break;
                        }
                    }
                    con_send(client, String("start"));
                    con_send(players[i].socket, String("start"));
                    wait = 0;
                }
                pthread_mutex_unlock(&mutex);
            }
        }
}

void make_move(int player, int where) {
    pthread_mutex_lock(&mutex);
    for (int i = 1; i < MAX_PLAYERS; ++i) {
        if (games[i].player1 == player) {
            games[i].board[where] = games[i].mode == PL1_CROSS ? 'X' : 'O';
        }
        if (games[i].player2 == player) {
            games[i].board[where] = games[i].mode == PL2_CROSS ? 'X' : 'O';
        }
    }
    pthread_mutex_unlock(&mutex);
}

Connection * server;

void * connection_thread(void * args) {
    while (1) {
        //sleep(1);
        printf("waiting na conn\n");
        int client = con_accept(server, new (sockaddr_in));
        int place = save_username_at(con_receive(client), client);
        printf("Connected\n");
        if (place == -1) {
            con_send(client, String("badusr"));
            shutdown(client, SHUT_RDWR);   
        }
        else {
            if (!players[place].is_in_game)
                create_and_run_thread(search_for_player, &client);
        }
    }
}

char * parse_response(char * msg, int player) {
    char * key;
    char * data;
    key = strtok(msg, "|");
    if (!strcmp(key, "move")) {
        int where = atoi(strtok(NULL, "|"));
        make_move(players, where);
    }
    else return key;
    
}

void * response_thread(void * args) {
    while (1) {
        //sleep(1);
        for (int i = 1; i < MAX_PLAYERS; ++i) {
            if (players[i].socket != 0) {
                char * response = con_receive(players[i].socket);
                if (response != NULL) {
                    parse_response(response, i);
                }
            }
        }
    }
}

void add_to_epoll(int desc) {
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, desc, &event)) //0 - deskryptor
    {
        fprintf(stderr, "Failed to add file descriptor %d to epoll\n", desc);
        close(epoll_fd);
        exit(1);
    }
}

char * send_board(char * str, int i) {
    char * response = New (char, 64);
    sprintf(response, "%s|X%c%c%c%c%c%c%c%c|", str, games[i].board[1], games[i].board[2], games[i].board[3], games[i].board[4], games[i].board[5], games[i].board[6], games[i].board[7], games[i].board[8]);
    return response;            
}

int main(int argc, char ** argv) {
    epoll_fd = epoll_create1(0);
    if(epoll_fd == -1)
    {
        fprintf(stderr, "Failed to create epoll file descriptor\n");
        return 1;
    }
    
    event.events = EPOLLIN;
    event.data.fd = 0;
    
    


    int port = atoi(argv[1]);

    server = new_connection(port, MAX_MSG_LEN, SERWER_IP, MAX_CONNECTION);
     
    con_listen(server);
    create_and_run_thread(connection_thread, NULL);
    while (1) {
        sleep(1);
        printf("server loop\n");
        pthread_mutex_lock(&mutex);
        for (int i = 1; i < MAX_PLAYERS; ++i) {
            if (games[i].player1 != 0) {               
                char * resp1 = send_board("map", i);
                char * resp2 = send_board("mapv", i);
                
                con_send(players[games[i].player1].socket, resp1);
                con_send(players[games[i].player2].socket, resp2);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}