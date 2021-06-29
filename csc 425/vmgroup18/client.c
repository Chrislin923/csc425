/*
* Authors: Evan Ellingsberg, Chris Lin
* CSC 425, Spring 2021
* Project 1 Milestone simple client code for TCP application. 
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>	
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc , char *argv[]){
    char *message= NULL;
    size_t msg_size = 1024;
    char full_mess[1028];
    int s;
    uint32_t length; 
    struct sockaddr_in server;
    char *sip = argv[1];
    int sport= atoi(argv[2]);
    s = socket(PF_INET, SOCK_STREAM, 0);
    inet_aton(sip, &server.sin_addr);
	server.sin_family = PF_INET;
	server.sin_port = htons(sport);
 
    connect(s , (struct sockaddr *)&server , sizeof(server));
  

    while(1){
        message = NULL;
        getline(&message, &msg_size, stdin);

        for (length = 0; message[length] != '\0'; ++length);
        length -=1;
        if((int)length < 0){
            break;
        }

        if(1024 < length) length = 1024;
        memcpy(full_mess, (char *)&length, sizeof(uint32_t));
        memcpy(&full_mess[4], message, (length)* sizeof(char));


        if(send(s , full_mess, length + 4, 0) <0){
            printf("Failed to send");
            return 1;
        }

    }
    close(s);
    return 0;



} 