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
    set_IPv4_socket_address("142.250.97.147", 80, &address);

    int result = connect(socketFD, (struct sockaddr*) &address, sizeof(address));

    if(result != 0) {
        perror("Connection not successful!\n");
        exit(1);
    }
    
    printf("Connection successful\n");

    char *message;
    message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n";
    send(socketFD, message, strlen(message), 0);

    char buffer[1024];
    recv(socketFD, buffer, 1024, 0);

    printf("Response was %s\n",buffer);


    return 0;
}