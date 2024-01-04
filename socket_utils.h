#ifndef MY_SOCKET_UTILS_H
#define MY_SOCKET_UTILS_H

#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void set_IPv4_socket_address(char* ip, int port, struct sockaddr_in* address)
{
    address->sin_family = AF_INET;
    //htons - makes sure that bytes are stored in big-endian (the standard network byte order)
    address->sin_port = htons(port);

    if (strlen(ip) == 0) {
        address->sin_addr.s_addr = INADDR_ANY;
    } 
    else {
        //inet_pton converts from representation format x.x.x.x to bytes in right order
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);
    }
}

int create_IPv4_socketFD() {
    //making socket file descriptor
    //domain: AF_INET - IPv4 Internet protocols,
    //type: SOCK_STREAM - TCP
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socketFD < 0) {
        perror("ERROR: can't open socket");
        exit(1);
    }

    return socketFD;
}



#endif