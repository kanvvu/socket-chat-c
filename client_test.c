#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "socket_utils.h"

void *recv_msg_from_server(void *socketFD) {
    int socket = (int) socketFD;

    while(1) {
        char buffer[1024];
        if (recv(socket, buffer, 1024, 0) > 0){
            printf("Response was: %s\n",buffer);
        }

        pthread_testcancel();
    }
}

int main() {
    int socketFD = create_IPv4_socketFD();

    struct sockaddr_in address;
    set_IPv4_socket_address("127.0.0.1", 2069, &address);

    int result = connect(socketFD, (struct sockaddr*) &address, sizeof(address));

    if(result != 0) {
        perror("Connection not successful!");
        exit(1);
    }
    
    printf("Connection successful\n");

    char *message;
    message = "Hi! I am kanku!\n";
    int send_result = send(socketFD, message, strlen(message), 0);

    if(send_result == -1) {
        perror("Sending not successful!");
        exit(1);
    }

    pthread_t thread;

    if(pthread_create(&thread, NULL, recv_msg_from_server, (void *) socketFD) != 0) {
        perror("Creating thread not successful!");
        exit(1);
    }

    while(1) {
        char *new_message = NULL;
        size_t msg_len = 0;
        printf("Write message: ");
        getline(&new_message, &msg_len, stdin);
        //printf("Writed msg: %s\n", new_message);

        send_result = send(socketFD, new_message, msg_len, 0);
        free(new_message);

        if(send_result == -1) {
            perror("Sending not successful!\n");
            exit(1);
        }
    }
    
    pthread_cancel(thread);
    pthread_join(thread, NULL);


    return 0;
}