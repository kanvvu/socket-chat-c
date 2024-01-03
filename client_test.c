#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

int main() {
    //making socket file descriptor
    //domain: AF_INET - IPv4 Internet protocols,
    //type: SOCK_STREAM - TCP
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    char* ip = "172.217.193.106";
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    //htons - makes sure that bytes are stored in big-endian (the standard network byte order)
    address.sin_port = htons(80);
    //inet_pton converts from representation format x.x.x.x to bytes in right order
    inet_pton(AF_INET, ip, &address.sin_addr.s_addr);

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

    printf("Response was %s\n,buffer");


    return 0;
}