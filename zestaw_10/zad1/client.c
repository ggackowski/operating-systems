#include "util.c"

pthread_t ** threads;
int thread_cnt = 0;

void create_and_run_thread(void * (*f) (void *), void * args) {
    pthread_t * thread = new (pthread_t);
    pthread_create(thread, NULL, f, args);
    threads[thread_cnt++] = thread;
}

int server_socket;

void sigint(int sig){
	shutdown(server_socket, SHUT_RDWR);
	exit(0);
}

int connect_to_server(int port, char * ip) {
    sockaddr_in * address = new (sockaddr_in);
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &(address->sin_addr)) < 0) error("inet_pton");
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (socket < 0) error("socket");
    if (connect(server_socket, (struct sockaddr *) address, sizeof(*address)) < 0) error("connect");
    return server_socket;
}

char * board;

char * parse_response(char * msg) {
    if (msg == NULL || !strcmp(msg, "")) return NULL;
    char * key;
    key = strtok(msg, "|");
    if (key != NULL && (!strcmp(key, "mapv") || !strcmp(key, "map"))) {
        strcpy(board, strtok(NULL, "|"));
    }
    return key; 
}

int state = NONE;

void * f(void * sockt) {
    int sckt = *((int *) sockt);
    while (1) {
        char * response = parse_response(con_receive(sckt));
        if (response == NULL) continue;
        if (!strcmp(response, "search")) 
            state = PLAYER_SEARCH;
        if (!strcmp(response, "start")) 
            state = GAME_STARTED;
        if (!strcmp(response, "badusr")) 
            state = BAD_USERNAME;
        if (!strcmp(response, "map")) 
            state = GAME_REFRESH;
        if (!strcmp(response, "mapv")) 
            state = GAME_REFRESH_MOVE;
        if (!strcmp(response, "ping")) 
            con_send(server_socket, "ping|");
        if (!strcmp(response, "win") || !strcmp(response, "lose")) {
            printf("You %s!", response);
            sigint(0);
        }
    }
}

void print_board() {
    system("clear");
    printf("%c|%c|%c\n- - -\n%c|%c|%c\n- - -\n%c|%c|%c\n", board[0], board[1], board[2], board[3], board[4], board[5], board[6], board[7], board[8]);
}

int main(int argc, char ** argv) {
    signal(SIGINT, sigint);
    char * username = argv[1];
    threads = New (pthread_t *, 32);
    char * ip_server_address = argv[3];
    int port_num = atoi(argv[4]);
    board = New (char, 9);
    for (int i = 0; i < 9; ++i) board[i] = ' ';
    char buffer[128];
    server_socket = connect_to_server(port_num, ip_server_address);
    char * name = New (char, 64);
    sprintf(name, "name|%s|", username);
    con_send(server_socket, name);
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
                print_board();
                break;  
            case BAD_USERNAME:
                printf("this username is not free.\nStopping..\n");
                sigint(0);
                break;
            case GAME_REFRESH:
                print_board();
                break;
            case GAME_REFRESH_MOVE:
                print_board();
                printf("1-9:\n");
                int response;
                scanf("%d", &response);
                char * msg = New (char, 64);
                sprintf(msg, "move|%d|", response);
                con_send(server_socket, msg);
                break;
            default:
                break;
        }
        while (old_state == state);
    }
    shutdown( server_socket, SHUT_RDWR);
    return 0;
}