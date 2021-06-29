/*
* Authors: Evan Ellingsberg, Chris Lin
* CSC 425, Spring 2021
* Project 1 Milestone simple server code for TCP application. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Example use: ./server portnum");
        return(1);
    }
    int sport = atoi(argv[1]);
    struct sockaddr_in server, client;
    char client_msg[1028];

    int s = socket(PF_INET, SOCK_STREAM, 0);
    if(s == -1){
        printf("Socket creation failed.");
    }
       ```     v   
    // Set server address structure
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(sport);

    // Bind socket to server
    bind(s, (struct sockaddr *)&server, sizeof(server));
    
    // Listen
    listen(s, 1);

    int c_socket = accept(s, (struct sockaddr *)&client, (socklen_t*)&c_socket);
    int bytes_read;
    while((bytes_read = recv(c_socket, client_msg, 1028, 0)) > 0){
        char lengthVal[4];
        memcpy(lengthVal, client_msg, 4);
        int length = (int)(*(uint32_t *)(lengthVal));
        char msg_val[length];
        msg_val[length] = '\0';
        
        memcpy(msg_val, &client_msg[4], length * sizeof(char));
        printf("%d\n", *(uint32_t *)(lengthVal));
        printf("%s\n", msg_val);
    }
    close(s);
}