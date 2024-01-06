
#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "socket_utils.h"

#include <pthread.h>

struct client_id
{
    int socketFD;
    int id;
    char nickname[20];
};

struct client_id clients[10];
int clients_num = 0;

void send_msg_to_others(char message[1024], int id, struct client_id client){
    char new_buff[1024] = "";
    sprintf(new_buff, "%s: %s", client.nickname, message);

    for(int i=0; i<clients_num; i++){
        if (client.id == i) continue;

        int send_result = send(clients[i].socketFD, new_buff, sizeof(new_buff), 0);
        if(send_result == -1) {
            perror("Sending not successful!");
            exit(1);
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
        else if(recv_result == 0){
            printf("Client (%d) '%s', disconnected", client.id, client.nickname);
            break;
        }

        pthread_testcancel();
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
    char client_nickname[20];
    if (recv(client_socketFD, client_nickname, 20, 0) > 0) 
        strcpy(&clients[clients_num].nickname,&client_nickname);
    printf("REC NICKNAME: %s\n", clients[clients_num].nickname);

    clients[clients_num].socketFD = client_socketFD;
    clients[clients_num].id = clients_num;

    pthread_t client_thread;

    if(pthread_create(&client_thread, NULL, recv_msg_from_client, (void *) &clients[clients_num]) != 0) {
        perror("Creating thread not successful!");
        exit(1);
    }

    clients_num += 1;
}

int main() {
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