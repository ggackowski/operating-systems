#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <arpa/inet.h> // inet_pton()
#include <sys/socket.h>

#define SERWER_PORT 8888
#define SERWER_IP "127.0.0.1"


int main()
{
    struct sockaddr_in serwer =
    {
        .sin_family = AF_INET,
        .sin_port = htons( SERWER_PORT )
    };
    if( inet_pton( AF_INET, SERWER_IP, & serwer.sin_addr ) <= 0 )
    {
        perror( "inet_pton() ERROR" );
        exit( 1 );
    }
   
    const int socket_ = socket( AF_INET, SOCK_DGRAM, 0 );
    if( socket_ < 0 )
    {
        perror( "socket() ERROR" );
        exit( 1 );
    }
   
    char buffer[ 4096 ] = "Message from client";
    printf( "|Message for server|: %s \n", buffer );
   
    socklen_t len = sizeof( serwer );
   
    if( sendto( socket_, buffer, strlen( buffer ), 0,( struct sockaddr * ) & serwer, len ) < 0 )
    {
        perror( "sendto() ERROR" );
        exit( 1 );
    }
   
    struct sockaddr_in from = { };
   
    memset( buffer, 0, sizeof( buffer ) );
    if( recvfrom( socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) & from, & len ) < 0 )
    {
        perror( "recvfrom() ERROR" );
        exit( 1 );
    }
    printf( "|Server's reply|: %s \n", buffer );
   
    shutdown( socket_, SHUT_RDWR );
}