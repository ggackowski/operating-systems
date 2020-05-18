#include "definitions.h"

char * String(char * s) {
    char * str = New(char, MAX_MSG_LEN);
    strcpy(str, s);
    return str;
} 

int con_accept(Connection * conn, sockaddr_in * client) {
    int len = sizeof(struct sockaddr_in *);
    int a = accept(conn->connection_socket, (sockaddr_in *) client, &len);
    if (a < 0) error("accept");
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

void con_listen(Connection * conn) {
    if (listen(conn->connection_socket, conn->max_conn) < 0) error("listen");
}