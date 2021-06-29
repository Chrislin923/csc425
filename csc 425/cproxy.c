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
        printf("Example use: ./cproxy portnum\n");
        return(1);
    }


	int t_port = atoi(argv[1]);
	char *sip = argv[2];
	int sp_port = atoi(argv[3]);


    struct sockaddr_in telnet, cp, sp;

    int n, sock
	char t_buf[1028], sp_buf[1028];
    fd_set readfds;



    int t_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(t_socket == -1){
        printf("Telnet Socket creation failed.");
    } 
    int s_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(s_socket == -1){
        printf("Sproxy Socket creation failed.");
    } 

    telnet.sin_family = PF_INET;
    telnet.sin_addr.s_addr = INADDR_ANY;
    telnet.sin_port = htons(t_port);
    bzero(&telnet.sin_zero, sizeof(telnet));


    bind(t_socket, (struct sockaddr *)&telnet, sizeof(telnet));

    listen(t_socket, 1);


	inet_aton(sip, &sp.sin_addr);
    sp.sin_family = PF_INET;
    sp.sin_port = htons(sp_port);
    bzero(&sp.sin_zero, sizeof(sp));

    int telnet_s = accept(t_socket, (struct sockaddr *)&cp, (socklen_t*)&telnet_s);

    int retRev; 

    retRev = connect(s_socket, (struct sockaddr *)&sp , sizeof(sp));

    if(retRev < 0){
        printf("Could not connect to the sproxy\n");
        exit(1);
    }

    FD_ZERO(&readfds);
    FD_SET(telnet_s, &readfds);
    FD_SET(s_socket, &readfds);

	int fdMax = s_socket + 1;
	int tRead, spRead = 0;

	while(retRev = select(fdMax, &readfds, NULL, NULL, NULL) >= 0){
		memset(t_buf, 0, sizeof(t_buf));
        memset(sp_buf, 0, sizeof(sp_buf));

        if (FD_ISSET(telnet_s, &readfds)) {
			tRead = recv(telnet_s, t_buf, 1028, 0);
			if(tRead <= 0){
                printf("Telnet Connection Closed\n");
                break;
            }

		}
		if (FD_ISSET(s_socket, &readfds)) {
			spRead = recv(s_socket, sp_buf, 1028, 0);
			
		}

		if(tRead >= 1){
			send(telnet_s, t_buf, sizeof(t_buf), 0);

		}
		else{
			printf("Telnet Connection Closed\n");
            return;
		}
		if(spRead >= 1){
            send(s_socket, sp_buf, sizeof(sp_buf), 0);
        }else{
            printf("sproxy Connection Closed\n");
            break;
        }
        FD_ZERO(&readfds);
        // Replace the cproxy and telnet daemon fd in the select set
        FD_SET(telnet_s, &readfds);
        FD_SET(s_socket, &readfds);

        if(telnet_s + 1 <= s_socket + 1) {
            fdMax = s_socket + 1;
        }else{
            fdMax = telnet_s + 1;
        }
        


	}


	


    close(t_socket);
    close(s_socket);


}
