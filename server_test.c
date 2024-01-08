
#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "socket_utils.h"
#include <signal.h>

#include <pthread.h>
#include <errno.h>

#define MAX_CLIENTS 10

struct client_id
{
    int socketFD;
    int id;
    int is_connected;
    char nickname[20];
    
};

struct client_id clients[MAX_CLIENTS];

void client_id_struct_init() {
    for(int i = 0; i<MAX_CLIENTS; i++ ) {
        clients[i].socketFD = 0;
        clients[i].id = i;
        clients[i].is_connected = 0;
        strcpy(clients[i].nickname, ""); 
    }
}

void send_msg_to_others(char message[1024], int id, struct client_id client){
    char new_buff[1024] = "";
    sprintf(new_buff, "%s: %s", client.nickname, message);

    for(int i=0; i<MAX_CLIENTS; i++){
        if (client.id == i || clients[i].is_connected == 0) continue;

        int send_result = send(clients[i].socketFD, new_buff, sizeof(new_buff), 0);
        if(send_result < 0) {
            char error_msg[256];
            sprintf(error_msg, "ERROR Sending to client (%d) not successful!", i);
            perror(error_msg);
        }
    }        

}

void * recv_msg_from_client(void* socketFD){
    struct client_id client = *((struct client_id*) socketFD);
    while(1) {
        char buffer[1024];
        int recv_result = recv(client.socketFD, buffer, 1024, 0);
        if (recv_result > 0) {
            printf("Message from client (%d): %s",client.id, buffer);
            send_msg_to_others(buffer, client.id, client);
        }
        if(recv_result < 0) {
            perror("ERROR recv msg from client");
        }
        if(recv_result == 0) {
            printf("Client (%d) disconected!\n", client.id);
            clients[client.id].is_connected = 0;
            break;
        }
    }
}

void accept_client(int socketFD){
    struct sockaddr_in client_addr;
    int clinet_addr_len = sizeof(client_addr);
    int client_socketFD = accept(socketFD,(struct sockaddr*) &client_addr, &clinet_addr_len);
    if (client_socketFD < 0) {
        perror("ERROR couldn't accept");
        exit(1);
    }
    printf("-ACCEPTED-\n");

    for(int i=0; i<MAX_CLIENTS; i++){
        if(!clients[i].is_connected) {
            clients[i].socketFD = client_socketFD;
            clients[i].is_connected = 1;
            printf("Client (%d) conected!\n", clients[i].id);

            char client_nickname[20];
            if (recv(client_socketFD, client_nickname, 20, 0) > 0) 
                strcpy(clients[i].nickname, client_nickname);
            printf("RECV NICKNAME: %s\n", clients[i].nickname);

            pthread_t client_thread;
            if(pthread_create(&client_thread, NULL, recv_msg_from_client, (void *) &clients[i]) != 0) {
                perror("Creating thread not successful!");
                exit(1);
            }

            break;
        }
    }

}

int main() {
    //instead of terminating program when send gives error, show error
    signal(SIGPIPE, SIG_IGN);

    client_id_struct_init();

    int socketFD = create_IPv4_socketFD();

    struct sockaddr_in address;
    set_IPv4_socket_address("", 2069, &address);

    if (bind(socketFD, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("ERROR couldn't bind");
        exit(1);
    }
    printf("-BIND SUCCESSFUL-\n");

    while(1)
    {
        printf("-LISTENING-\n");

        listen(socketFD, 5);

        accept_client(socketFD);

    }

    close(socketFD);
    shutdown(socketFD, SHUT_RDWR);

    printf("SERVER SHUTDOWN! \n");


    return 0;
}