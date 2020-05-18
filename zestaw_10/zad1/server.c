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
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>

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
    int tour;
} Game;

Game games[MAX_PLAYERS];

pthread_t ** threads;
int thread_cnt = 0;

void create_and_run_thread(void * (*f) (void *), void * args) {
    pthread_t * thread = new (pthread_t);
    pthread_create(thread, NULL, f, args);
    threads[thread_cnt++] = thread;
}


typedef struct {
    sockaddr_in address;
    int connection_socket;
    int port;
    int max_mess_len;
    int max_conn;
    char ip[64];
} Connection;

Connection * server;

char * String(char * s) {
    char * str = New(char, MAX_MSG_LEN);
    strcpy(str, s);
    return str;
} 

void error(char * msg) {
    printf("err: %s with errno %d (%s)\n", msg, errno, strerror(errno));
    sigint(0);
}

void sigint(int sig){
    for (int i = 0; i < thread_cnt; ++i) 
        pthread_kill(*(threads[i]), SIGINT);
	shutdown(server->connection_socket, SHUT_RDWR);
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (players[i].socket > 0)
            shutdown(players[i].socket, SHUT_RDWR);
    } 
	exit(0);
}


struct epoll_event event, events[MAX_PLAYERS];
int  epoll_fd;
int epoll_max;

Connection * new_connection(int port, int max_mess_len, char * ip, int max_conn) {
    Connection * conn = new (Connection);
    conn->address.sin_family = AF_INET;
    conn->address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &conn->address.sin_addr) <= 0) error("inetpton");
    conn->connection_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) error("socket error");
    conn->max_conn = max_conn;
    if (bind(conn->connection_socket, (struct sockaddr_in *) &(conn->address), sizeof(conn->address)) < 0) error("bind");
    return conn;
}

void con_listen(Connection * conn) {
    if (listen(conn->connection_socket, conn->max_conn) < 0) error("listen");
}

sockaddr_in * new_address() {
    return NULL;
}

void add_to_epoll(int desc) {
    struct epoll_event ev = { .events = EPOLLIN | EPOLLPRI };
    ev.data.fd = desc;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, desc, &ev)) //0 - deskryptor
    {
        fprintf(stderr, "Failed to add file descriptor %d to epoll\n", desc);
        close(epoll_fd);
        sigint(0);
    }
}

int con_accept(Connection * conn, sockaddr_in * client) {
    printf("accepting now\n");
    int len = sizeof(struct sockaddr_in *);
    int a = accept(conn->connection_socket, (sockaddr_in *) client, &len);
    if (a < 0) error("accept");
    printf("accepted\n");
    return a;
}

char * con_receive(int socket) {
    char * message = New (char, MAX_MSG_LEN);
    if (recv(socket, message, MAX_MSG_LEN, 0) == -1)
        return NULL;
    return message;
}

void con_send(int socket, char * message) {
    printf("IM RIGHT NOW SENDING >%s< TO %d\n", message, socket);
    send( socket, message, strlen(message), 0 );
}

int save_username_at(char * str, int client) {
    printf("FZIEEEEEEEEEE %s, %d\n", str, client);
    if (str == NULL) return -1;
    for (int i = 1; i < MAX_PLAYERS; ++i) {
        if (players[i].socket == -1) {
            
            players[i].socket = client;
            players[i].username = String(str);
            printf("umieszczam socket %d w %d\n", client, i);          
            return i;   
        }
        if (!strcmp(players[i].username, str))
            return -1;
        
    }
    return -1;
}

char * send_board(char * str, int i) {
    char * response = New (char, 64);
    sprintf(response, "%s|%c%c%c%c%c%c%c%c%c|", str, games[i].board[0], games[i].board[1], games[i].board[2], games[i].board[3], games[i].board[4], games[i].board[5], games[i].board[6], games[i].board[7], games[i].board[8]);
    return response;            
}

void * search_for_player(void * cl) {
    printf("search for player\n");
    for (int i = 1; i < MAX_PLAYERS; ++i) {
        printf("%s ", players[i].username);
    }
    int client = *((int *) cl);
    printf("moj socket etraz: %d\n", client);
    int place = -1;
    con_send(client, String("search"));
    int game_id = -1;
    for (int i = 1; i < MAX_PLAYERS; ++i) if (client == players[i].socket) place = i;
    printf("mam gdzie jest gracz: %d\n", place);
        int wait = 1;
        while (wait) {
            for (int i = 1; i < MAX_PLAYERS && wait; ++i) {
                //pthread_mutex_lock(&mutex);
                if (!players[i].is_in_game && i != place && players[i].socket != -1) {
                    printf("mam gracza %d, %s\n", i, players[i].username);
                    players[i].is_in_game = 1;
                    players[place].is_in_game = 1;
                    for (int j = 1; j < MAX_PLAYERS; ++j) {
                        if (games[j].player1 == -1) {
                            games[j].board[0] = games[j].board[1] = games[j].board[2] = games[j].board[3] = games[j].board[4] = games[j].board[5] = games[j].board[6] = games[j].board[7] = games[j].board[8] = ' '; 
                            games[j].player1 = place;
                            games[j].player2 = i;
                            games[j].mode =  rand() % 2 ? PL1_CROSS : PL2_CROSS;
                            games[j].tour = rand() % 2 ? PL1 : PL2;
                            game_id = j;
                            break;
                        }
                    }
                    if (games[game_id].tour == PL1) {
                        con_send(players[games[game_id].player1].socket, send_board("mapv", game_id));
                        con_send(players[games[game_id].player2].socket, send_board("map", game_id));
                    }
                    else {
                        con_send(players[games[game_id].player1].socket, send_board("map", game_id));
                        con_send(players[games[game_id].player2].socket, send_board("mapv", game_id));
                    }
                    wait = 0;
                    printf("found a game\n");
                }
            }
        }
}

void check_win(int game_id, int i) {
    if (games[game_id].board[i] == 'X' && games[game_id].mode == PL1_CROSS) {
                con_send(players[games[game_id].player1].socket, "win||");
                con_send(players[games[game_id].player2].socket, "lose||");
            }
            else if (games[game_id].board[i] == 'X' && games[game_id].mode == PL2_CROSS) {
                con_send(players[games[game_id].player1].socket, "lose||");
                con_send(players[games[game_id].player2].socket, "win||");
            }
            else if (games[game_id].board[i] == 'O' && games[game_id].mode == PL1_CROSS) {
                con_send(players[games[game_id].player1].socket, "lose||");
                con_send(players[games[game_id].player2].socket, "win||");
            }
            else if (games[game_id].board[i] == 'O' && games[game_id].mode == PL2_CROSS) {
                con_send(players[games[game_id].player1].socket, "win||");
                con_send(players[games[game_id].player2].socket, "lose||");
            }
}

void make_move(int player, int where) {
    pthread_mutex_lock(&mutex);
    int game_id = -1;
    for (int i = 1; i < MAX_PLAYERS; ++i) {
        if (games[i].player1 == player) {
            games[i].board[where] = games[i].mode == PL1_CROSS ? 'X' : 'O';
            game_id = i;
            break;
        }
        if (games[i].player2 == player) {
            games[i].board[where] = games[i].mode == PL2_CROSS ? 'X' : 'O';
            game_id = i;
            break;
        }
        
    }
    int win = 0;
    for (int i = 0; i < 3; ++i) {
        //horizontal
        if (games[game_id].board[3 * i] != ' ' && games[game_id].board[3 * i] == games[game_id].board[3 * i + 1] && games[game_id].board[3 * i + 1] == games[game_id].board[3 * i + 2]) {
            check_win(game_id, 3 * i);
            win = 1;
        }
        else if (games[game_id].board[i] != ' ' && games[game_id].board[i] == games[game_id].board[i + 3] && games[game_id].board[i + 3] == games[game_id].board[i + 6]) {
            check_win(game_id, i);
            win = 1;
        }
        else if (games[game_id].board[2] != ' '  && games[game_id].board[2] == games[game_id].board[4] && games[game_id].board[4] == games[game_id].board[6]) {
            check_win(game_id, 2);
            win = 1;
        }
        else if (games[game_id].board[0] != ' ' && games[game_id].board[0] == games[game_id].board[4] && games[game_id].board[4] == games[game_id].board[8]) {
            check_win(game_id, 0);
            win = 1;
        }
    }
    if (win) {
        printf("WIN\n");
    }
    if (!win) {
    games[game_id].tour = games[game_id].tour == PL1 ? PL2 : PL1;
    if (games[game_id].tour == PL1) {
        con_send(players[games[game_id].player1].socket, send_board("mapv", game_id));
        con_send(players[games[game_id].player2].socket, send_board("map", game_id));
    }
    else {
        con_send(players[games[game_id].player1].socket, send_board("map", game_id));
        con_send(players[games[game_id].player2].socket, send_board("mapv", game_id));
    }
    }
    pthread_mutex_unlock(&mutex);
}



void * connection_thread(void * args) {
    while (1) {
        printf("waiting na conn\n");
        int client = con_accept(server, new (sockaddr_in));
        add_to_epoll(client);
        printf("added to epoll\n");   
    }
}

int adding_player;

int responded[MAX_PLAYERS];

void * ping(void * _) {
    while (1) {
        for (int i = 1; i < MAX_PLAYERS; ++i)
            responded[i] = 0;
        for (int i = 1; i < MAX_PLAYERS; ++i) {
            if (players[i].socket != -1) {
                con_send(players[i].socket, "ping|");
            }
        }
        sleep(5);
        for (int i = 1; i < MAX_PLAYERS; ++i)
            if (responded[i] == 0 && players[i].socket != -1) {
                printf("%d not responding \n", i);
            }

    }
}

char * parse_response(char * msg, int player) {
    printf("%s\n", msg);
    char * key;
    char * data;
    key = strtok(msg, "|");
    if (!strcmp(key, "move")) {
        int where = atoi(strtok(NULL, "|"));
        make_move(player, where);
    }
    if (!strcmp(key, "name")) {
        char * name = strtok(NULL, "|");
        int place = save_username_at(name, player);
        if (place == -1) {
            con_send(player, String("badusr"));
            shutdown(player, SHUT_RDWR);   
        }
        else {
                adding_player = player;
                create_and_run_thread(search_for_player, &adding_player);
        } 
    }
    if (!strcmp(key, "ping")) {
        responded[player] = 1; 
    }
    else return key;
    
}

int main(int argc, char ** argv) {
    for (int i = 1; i < MAX_PLAYERS; ++i)  {players[i].socket = -1; games[i].player1 = -1; games[i].player2 = -1; }
    srand(time(NULL));
    signal(SIGINT, sigint);
    threads = New (pthread_t *, 32);
    epoll_fd = epoll_create1(0);
    if(epoll_fd == -1)
    {
        fprintf(stderr, "Failed to create epoll file descriptor\n");
        sigint(0);
    }
    
    event.events = EPOLLIN;
    event.data.fd = MAX_PLAYERS;
    event.events = EPOLLIN | EPOLLPRI;
    epoll_max = 0;
    int port = atoi(argv[1]);

    server = new_connection(port, MAX_MSG_LEN, SERWER_IP, MAX_CONNECTION);
     
    con_listen(server);
    create_and_run_thread(connection_thread, NULL);
    create_and_run_thread(ping, NULL);
    while (1) {
        //sleep(1);
        printf("server loop\n");
        int nfds = epoll_wait(epoll_fd, events, MAX_PLAYERS, 3000);
        if (nfds < 0) {
            printf("err epol wait\n");
            sigint(0);
        }
        //pthread_mutex_lock(&mutex);
        for (int i = 0; i < nfds; ++i) {
            printf("odczyt z socketu %d\n", i);
            int sckt = events[i].data.fd;
            printf("mam socket %d\n", sckt);
            int a_player = 0;
            for (int j = 0; j < MAX_PLAYERS; ++j) {
                if (sckt == players[j].socket) {
                    a_player = 1;
                    printf("socket nalezy do gracza %s\n", players[j].username);
                    char * response = con_receive(sckt);
                    if (response != NULL) {
                        parse_response(response, j);
                    }
                }
            }
            if (!a_player) {
                printf("dodaje gracza\n");
                char * response = con_receive(sckt);
                if (response != NULL) {
                    parse_response(response, sckt);
                }
            }
            printf("po dodaniu\n");
        }
    }
}