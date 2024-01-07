#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "socket_utils.h"

void *recv_msg_from_server(void *socketFD) {
    int socket = *((int *) socketFD);

    while(1) {
        char buffer[1024];
        if (recv(socket, buffer, 1024, 0) > 0){
            printf("\n%s", buffer);
        }

        pthread_testcancel();
    }
}

void send_messages(int socketFD){
    printf("Write message (type 'exit' to exit)\n");
    while(1) {
        char *new_message = NULL;
        size_t msg_len = 0;
        getline(&new_message, &msg_len, stdin);
        
        if(strcmp(new_message, "exit\n") == 0){
            break;
        }

        int send_result = send(socketFD, new_message, msg_len, 0);
        free(new_message);

        if(send_result == -1) {
            perror("Sending not successful!\n");
            exit(1);
        }
    }
}

void get_and_send_nickname(int socketFD){
    char nickname[20];
    printf("Set your nickname: ");
    scanf("%s", nickname);
    getchar();
    int send_result = send(socketFD, nickname, sizeof(nickname), 0);
    printf("NICKNAME SENT: %s\n", nickname);

    if(send_result == -1) {
        perror("Sending not successful!");
        exit(1);
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

    get_and_send_nickname(socketFD);

    pthread_t thread;
    if(pthread_create(&thread, NULL, recv_msg_from_server, (void *) &socketFD) != 0) {
        perror("Creating thread not successful!");
        exit(1);
    }
    
    send_messages(socketFD);

    pthread_cancel(thread);
    pthread_join(thread, 0);

    close(socketFD);
    printf("SUCCESSFULY EXITED!\n");

    return 0;
}