#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int main(int argc	, char *argv[]) {

	// Initialize sockets for each port
	int size=argc-1;
	short ports[size];
	int i=0;
	for(i=0;i<size;i++){
		ports[i]=atoi(argv[i+1]);
	}
	int sockets[size];
	char buffer[BUFFER_SIZE];
	struct sockaddr_in socketsAddr[size];
	int maxSocket=0;

	// Set up sockets, Bind
	for(i=0;i<size;i++){
		if((sockets[i] = socket(AF_INET,SOCK_DGRAM,0))<0){
			perror("Socket error: "); return 0;}
		if(sockets[i]>maxSocket) maxSocket=sockets[i];
		socketsAddr[i].sin_family = AF_INET;
		socketsAddr[i].sin_port = htons(ports[i]);
		socketsAddr[i].sin_addr.s_addr=inet_addr("192.6.2.1");
		if(bind(sockets[i],(struct sockaddr *)&socketsAddr[i],sizeof(socketsAddr[i]))<0){
			perror("Bind error: "); return 0;}
	}

	//Initialize file descriptor set
	fd_set current_sockets,ready_sockets;
	FD_ZERO(&current_sockets);
	for(i=0; i<size; i++){
		FD_SET(sockets[i],&current_sockets);
	}
	FD_SET(fileno(stdin), &current_sockets);

	puts("Listening...");

	while(1){
		ready_sockets = current_sockets;

		if(select(maxSocket+1,&ready_sockets, NULL, NULL, NULL) < 0){
			perror("Select Error. \n"); return 0;
		}

		if(FD_ISSET(fileno(stdin), &ready_sockets)){
			printf("A command was entered in the console! n");
			//recv(fileno(stdin),buffer,BUFFER_SIZE,0);
		}

		for (i=0; i< size; i++){
			if(FD_ISSET(sockets[i], &ready_sockets)){
				printf("Client with port %d has sent a message. \n",ports[i]);
				if(recv(sockets[i],buffer,BUFFER_SIZE,0)<0){
					puts("Receive error."); return 0;
				}
			}
		}
	}
}

