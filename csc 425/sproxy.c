/*
* Authors: Evan Ellingsberg, Chris Lin
* CSC 425, Spring 2021
* Project 1 Milestone 2 TCP proxy Implementation for the server proxy. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>


int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Example use: ./sproxy portnum\n");
        return(1);
    }
    int sport = atoi(argv[1]);
    struct sockaddr_in cproxyAddr, client, tdaemonAddr;
    char cproxy_buf[1028], tdaemon_buf[1028];
    fd_set readfds;

    int cpsock = socket(PF_INET, SOCK_STREAM, 0);
    if(cpsock == -1){
        printf("Socket creation failed.");
    }

    // Set cproxy address structure
    cproxyAddr.sin_family = PF_INET;
    cproxyAddr.sin_addr.s_addr = INADDR_ANY;
    cproxyAddr.sin_port = htons(sport);
    bzero(&cproxyAddr.sin_zero, sizeof(cproxyAddr));

    // Bind socket to cproxy
    bind(cpsock, (struct sockaddr *)&cproxyAddr, sizeof(cproxyAddr));

    // Set telnet daemon address structure
    tdaemonAddr.sin_family = PF_INET;
    int retval = inet_aton("127.0.0.1", &tdaemonAddr.sin_addr);
    if(retval == 0){
        printf("Error in setting telnet daemon address to 127.0.0.1\n");
        exit(1);
    }
    tdaemonAddr.sin_port = htons(23);
    bzero(&tdaemonAddr.sin_zero, sizeof(tdaemonAddr));

    int tdsock = socket(PF_INET, SOCK_STREAM, 0);
    if(tdsock == -1){
        printf("Telnet daemon socket failed to be created\n");
        exit(1);
    }
    
    // Listen for the cproxy connection
    listen(cpsock, 1);

    // Accept the cproxy connection
    int clientStructLen = sizeof(client);
    int cp_socket = accept(cpsock, (struct sockaddr *)&client, &clientStructLen);

    // Connect to telnet daemon when the cproxy connection occurs
    retval = connect(tdsock, (struct sockaddr *) &tdaemonAddr, sizeof(tdaemonAddr));
    if(retval == -1){
        printf("Could not connect to the Telnet Daemon\n");
        exit(1);
    }

    // Adding both the cproxy socket and telnet daemon socket to the select fd set
   
    int fdMax;
    // if(cp_socket < tdsock){
    //     fdMax = tdsock;
    // }else{
    //     fdMax = cp_socket;
    // }
    // fdMax += 1;
    int recieveValues = 1;
    int sendValues = 1;
    while((recieveValues > 0) && (sendValues > 0)){
        FD_ZERO(&readfds);
        FD_SET(cp_socket, &readfds);
        FD_SET(tdsock, &readfds);

        if(cp_socket + 1 < tdsock + 1) {
            fdMax = tdsock + 1;
        }else{
            fdMax = cp_socket + 1;
        }

        retval = select(fdMax, &readfds, NULL, NULL, NULL);
        // memset(cproxy_buf, 0, sizeof(cproxy_buf));
        // memset(tdaemon_buf, 0, sizeof(tdaemon_buf));
        // Check if the cproxy has sent data to the sproxy
        if(FD_ISSET(cp_socket, &readfds)){
            //bzero(cproxy_buf, sizeof(cproxy_buf));
            recieveValues = recv(cp_socket, cproxy_buf, sizeof(cproxy_buf), 0);
            printf("Recieved from clientside\n");
            if(recieveValues > 0){
                sendValues = send(tdsock, cproxy_buf, recieveValues, 0);
                printf("Sent the data from the Clientside");
                bzero(cproxy_buf, sizeof(cproxy_buf));
            }
            // if(recieveValues <= 0){
            //     printf("Cproxy Connection Closed in FD check\n");
            //     break;
            // }
        }

        // Check if the sproxy has its data to send to the telnet daemon
        if(FD_ISSET(tdsock, &readfds)){
            //bzero(tdaemon_buf, sizeof(tdaemon_buf));
            recieveValues = recv(tdsock, tdaemon_buf, sizeof(tdaemon_buf), 0);
            printf("Recieved from telnet daemon\n");
            if(sendValues > 0){
                sendValues = send(cp_socket, tdaemon_buf, recieveValues, 0);
                bzero(tdaemon_buf, sizeof(tdaemon_buf));
            }
        }

        // if(sendValues >= 1){
        //     send(cp_socket, tdaemon_buf, sizeof(tdaemon_buf), 0);
        // }else if(sendValues < 0){
        //     printf("Telnet Daemon Connection Closed\n");
        //     return;
        // }

        // if(recieveValues >= 1){
        //     send(tdsock, cproxy_buf, sizeof(cproxy_buf), 0);
        // }else if(recieveValues < 0){
        //     printf("Cproxy Connection Closed\n");
        //     break;
        // }
        
    }
    close(cpsock);
    close(tdsock);
    return 0;
}