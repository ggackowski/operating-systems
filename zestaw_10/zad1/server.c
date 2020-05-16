#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define MAX_MSG_LEN 4096
#define SERWER_PORT 8888
#define SERWER_IP "127.0.0.1"
#define MAX_CONNECTION 10
#define MAX_PLAYERS 16

#define new(Type) calloc(1, sizeof(Type))
#define New(Type, Size) calloc(Size, sizeof(Type))

typedef struct sockaddr_in sockaddr_in;

typedef struct {
    char * username;
    int socket;
    int is_in_game;
} client;

client players[16];

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
    return accept(conn->connection_socket, (sockaddr_in *) client, &len);
}

char * con_receive(int socket) {
    char * message = New (char, MAX_MSG_LEN);
    recv(socket, message, MAX_MSG_LEN, 0);
    return message;
}

void con_send(int socket, char * message) {
    send( socket, message, strlen(message), 0 );
}

int save_username_at(char * str, int client) {
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (players[i].socket == 0) {
            players[i].socket = client;
            players[i].username = String(str);
            return i;   
        }
    }
    return -1;
}

void * search_for_player(void * cl) {
    int client = *((int *) cl);
    int place = -1;
    con_send(client, String("search"));
    for (int i = 0; i < MAX_PLAYERS; ++i) if (client == players[i].socket) place = i;
        int wait = 1;
        while (wait) {
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                if (!players[i].is_in_game && i != place && players[i].socket != 0) {
                    //printf("%d %d %d", players[i].is_in_game, )
                    players[i].is_in_game = 1;
                    players[place].is_in_game = 1;
                    con_send(client, String("start"));
                    con_send(players[i].socket, String("start"));
                    wait = 0;
                }
            }
        }
}



int main(int argc, char ** argv) {
    int port = atoi(argv[1]);

    Connection * server = new_connection(port, MAX_MSG_LEN, SERWER_IP, MAX_CONNECTION);
    con_listen(server);
   
    while (1) {
        int client = con_accept(server, new (sockaddr_in));
        int place = save_username_at(con_receive(client), client);
        if (place == -1) {
            con_send(client, String("badusr"));
            shutdown(client, SHUT_RDWR);   
        }
    else {
        if (!players[place].is_in_game)
            create_and_run_thread(search_for_player, &client);

    }
    }

    /*
    
    //shutdown( clientSocket, SHUT_RDWR );
    //shutdown( socket_, SHUT_RDWR );
    */
}