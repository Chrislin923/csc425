/*
* Authors: Evan Ellingsberg, Chris Lin
* CSC 425, Spring 2021
* Project 2 Milestone cproxy to connect to telnet and sproxy
*/
 

#include <stdio.h>
#include <string.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>


int main(int argc , char *argv[]){

    if(argc < 2){
        printf("Example use: ./cproxy 5200 sip 6200\n");
        return(1);
    }


    int cport = atoi(argv[1]);
    char *sip = argv[2];
    int sp_port = atoi(argv[3]);


    struct sockaddr_in tAddr, cp, sp;

    char t_buf[1028], sp_buf[1028];
    fd_set readfds;



    int t_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(t_socket == -1){
        printf("Telnet Socket creation failed.");
    } 


    tAddr.sin_family = PF_INET;
   
    tAddr.sin_addr.s_addr = INADDR_ANY;
    tAddr.sin_port = htons(cport);
    bzero(&tAddr.sin_zero, sizeof(tAddr));


    bind(t_socket, (struct sockaddr *)&tAddr, sizeof(tAddr));

    
    sp.sin_family = PF_INET;
    int retval_sip = inet_aton(sip, &sp.sin_addr);
    
    if(retval_sip == 0){
        printf("Error in setting sproxy address \n");
        exit(1);
    }
    sp.sin_port = htons(sp_port);
    bzero(&sp.sin_zero, sizeof(sp));

    int s_socket = socket(PF_INET, SOCK_STREAM, 0);

    if(s_socket == -1){
        printf("Sproxy Socket creation failed.");
    } 



    listen(t_socket, 1);


    int cpStructLen = sizeof(cp);
    int telnet_s = accept(t_socket, (struct sockaddr *)&cp, &cpStructLen);

    int retRev; 

    retRev = connect(s_socket, (struct sockaddr *)&sp , sizeof(sp));

 
    if(retRev < 0){
        printf("Could not connect to the sproxy\n");
        exit(1);
    }



    int fdMax;

    int recieveValues = 1;
    int sendValues = 1;

    while((recieveValues > 0) && (sendValues > 0)){

        FD_ZERO(&readfds);
        FD_SET(telnet_s, &readfds);
        FD_SET(s_socket, &readfds);

        if(telnet_s + 1 < s_socket + 1) {
            fdMax = s_socket + 1;
        }else{
            fdMax = telnet_s + 1;
        }


        retRev = select(fdMax, &readfds, NULL, NULL, NULL);


        if (FD_ISSET(telnet_s, &readfds)) {
            recieveValues = recv(telnet_s, t_buf, sizeof(t_buf), 0);
            if(recieveValues > 0){
                sendValues = send(s_socket, t_buf, recieveValues, 0);
                printf("Recieved the data from the telnet");
                bzero(t_buf, sizeof(t_buf));
            }

        }
        if (FD_ISSET(s_socket, &readfds)) {
            recieveValues = recv(s_socket, sp_buf, sizeof(sp_buf), 0);
            printf("%d bytes Recieved from the sproxy\n", recieveValues);
            printf("Recieved from the sproxy\n");
            if(sendValues > 0){
                sendValues = send(s_socket, sp_buf, recieveValues, 0);
                bzero(sp_buf, sizeof(sp_buf));
            }
            
        }

        


    }


    


    close(t_socket);
    close(s_socket);


}
