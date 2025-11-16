#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

struct Node {
    int clientSocket;
    struct sockaddr_in clientSocketAddr;
    char name[6];
    socklen_t clientSocketAddrSize;
    struct Node* next;
};

#define BUFFER_SIZE 1024

int main(int argc	, char *argv[]) {

	// argv: 1)Server's port ,2)Max clients allowed
	if(argc>3){
		puts("Correct usage is: <Server's port> <Max clients allowed>");
		return 0;
	}


	// malloc max clients int**

	int i=0;
	int welcomeSocket;
	struct sockaddr_in welcomeAddr;
	struct Node *Head=NULL;
	int maxClients=atoi(argv[2]),Counter=0;
	short Port=atoi(argv[1]);

	/* - Socket -> Bind -> Listen = Welcome Socket
	 * while(select(Welcome Socket + stdin))
	 * 		If 0 - Check connections with recv
	 * 			If recv=-1, close connection, free space, announce
	 * 		If not 0,-1 - Accept new connection
	 * 			if max clients = deny connection
	 * 			else accept(), malloc
	 * 		If -1 - Error
	 * 		If stdin -> exit program, close all connections
	 */

	if((welcomeSocket = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("Socket error: "); return 0;}
		welcomeAddr.sin_family = AF_INET;
		welcomeAddr.sin_port = htons(Port);
		welcomeAddr.sin_addr.s_addr=inet_addr("192.6.2.1");

	if(bind(welcomeSocket,(struct sockaddr *)&welcomeAddr,sizeof(welcomeAddr))<0){
		perror("Bind error: "); return 0;}

	if(listen(welcomeSocket,1)<0){
		perror("Listen error: "); return 0;}

	//Initialize file descriptor set
	fd_set fdset,fdcopy;
	FD_ZERO(&fdset);
	FD_SET(welcomeSocket,&fdset);
	FD_SET(fileno(stdin), &fdset);

	char buffer[BUFFER_SIZE];

	int p;
	struct Node * tmp,* prev;
	fdcopy=fdset;
	puts("Server open...");
	while(1){
		fdset=fdcopy;
		p=select(FD_SETSIZE,&fdset, NULL, NULL, NULL);
		memset(buffer,BUFFER_SIZE,0);
		//puts("select jumped");
		if(p==-1){
			perror("Select error:"); //
			break;
		}

		if(FD_ISSET(welcomeSocket,&fdset)){
			FD_CLR(welcomeSocket,&fdset); // Lower flag
			// New connection
			if(Counter==maxClients){ // Deny
				puts("New connection denied, server full");

			}
			else{ // Adding new socket
				Counter++;
				tmp=Head;
				if(Head==NULL){ // First socket
					Head=malloc(sizeof(struct Node));
					Head->clientSocket=accept(welcomeSocket,(struct sockaddr *)&(Head->clientSocketAddr),&(Head->clientSocketAddrSize));
					Head->next=NULL;
					FD_SET(Head->clientSocket,&fdcopy);
					tmp=Head;

				}
				else{ // Few already exist
					while(tmp->next!=NULL) tmp=tmp->next;
					tmp->next=malloc(sizeof(struct Node)); tmp=tmp->next;
					tmp->clientSocket=accept(welcomeSocket,(struct sockaddr *)&(tmp->clientSocketAddr),&(tmp->clientSocketAddrSize));
					FD_SET(tmp->clientSocket,&fdcopy);
					tmp->next=NULL;
				}
				recv(tmp->clientSocket,buffer,BUFFER_SIZE,0);
				memcpy(tmp->name,buffer,6);
				//send(tmp->clientSocket,"Joined.",200,0); //send to the client that join right now
				printf("Client %s has connected. \n",tmp->name);

				prev=tmp;
				tmp=Head;
				while(tmp!=NULL){
					if(tmp==prev){
						tmp=tmp->next; continue;
					}
					send(tmp->clientSocket,buffer,BUFFER_SIZE,0);
					tmp=tmp->next;
				}
				continue;
			}
		}
		if(FD_ISSET(fileno(stdin),&fdset)){
			puts("Goodbye... "); break;
		}
		// Existing connection jumped -> FIN ACK
		if(Head==NULL){ puts("continued"); continue;}
		prev=Head;
		tmp=Head->next;
		if(FD_ISSET(Head->clientSocket,&fdset)){ // Maybe delete
			if(recv(Head->clientSocket,buffer,BUFFER_SIZE,0)<1){
				Head=Head->next;
				printf("Client %s disconnected. \n",prev->name);
				FD_CLR(prev->clientSocket,&fdcopy); close(prev->clientSocket); free(prev);
				Counter--;
				continue;
			}
			else{
				FD_CLR(Head->clientSocket,&fdset);
				tmp=Head->next;
				while(tmp!=NULL){
					send(tmp->clientSocket,buffer,BUFFER_SIZE,0);
					tmp=tmp->next;
				}
				continue;
			}
		}
		prev=Head;
		tmp=Head->next;
		while(tmp!=NULL){
			if(FD_ISSET(tmp->clientSocket,&fdset)){
				if(recv(tmp->clientSocket,buffer,BUFFER_SIZE,0)<1){ //delete tmp
					prev->next=tmp->next;
					printf("Client %s disconnected. \n",tmp->name);
					FD_CLR(tmp->clientSocket,&fdcopy); close(tmp->clientSocket); free(tmp);
					Counter--;
					break;
				}
				else{
					FD_CLR(tmp->clientSocket,&fdset);
					prev=tmp;
					tmp=Head;
					while(tmp!=NULL){
						if(tmp==prev) {tmp=tmp->next; continue;}
						send(tmp->clientSocket,buffer,BUFFER_SIZE,0);
						tmp=tmp->next;
					}
					continue;
				}
			}
			prev=tmp;
			tmp=tmp->next;
		}
	}
	tmp=Head;
	while(tmp!=NULL){
		close(tmp->clientSocket);
		prev=tmp;
		tmp=tmp->next;
		free(prev);
	}
	close(welcomeSocket);
}


