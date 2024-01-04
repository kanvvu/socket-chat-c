
#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "socket_utils.h"

int main() {
    int socketFD = create_IPv4_socketFD();

    struct sockaddr_in address;
    set_IPv4_socket_address("", 2069, &address);

    if (bind(socketFD, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("ERROR couldn't bind");
        exit(1);
    }
    printf("-BIND SUCCESSFUL-\n");

    printf("-LISTENING-\n");

    listen(socketFD, 5);

    struct sockaddr_in client_addr;
    int clinet_addr_len = sizeof(client_addr);
    int client_socketFD = accept(socketFD,(struct sockaddr*) &client_addr, &clinet_addr_len);
    if (client_socketFD < 0) {
        perror("ERROR couldn't accept");
        exit(1);
    }

    printf("-ACCEPTED-\n", client_addr.sin_addr.s_addr);


    char buffer[1024];
    if (recv(client_socketFD, buffer, 1024, 0) < 0) {
        perror("ERROR couldn't read from socket");
        exit(1);
    }

    printf("Message from client: %s\n", buffer);





    return 0;
}