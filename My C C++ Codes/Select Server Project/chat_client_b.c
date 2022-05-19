#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

char * buildMSG(char * buffer, int argc, char *argv[]){
		buffer=strcpy(buffer,argv[3]); // Name
		unsigned long length=strlen(argv[4]);
		*((unsigned long*)(buffer+6)) = length;
		strcpy((buffer+8),argv[4]); // MSG
		return buffer;
}


int main(int argc, char *argv[]) {

	//argv: 1) Server's IP address, 2) Port, 3)Name, 4)Message

	if(argc>5){
		puts("Correct usage is: <Server IP Address> <Port> <Name> <''Message (in quotations)''>");
		return 0;
	}
	int i=0;
	unsigned long length;
	short Port=atoi(argv[2]);
	char * buffer = malloc(sizeof(char)*BUFFER_SIZE);
	buffer=buildMSG(buffer,argc,argv);

	int ClientSocket;
	struct sockaddr_in serverAddr;

	// Send initial message - socket -> connect -> send
	// ================================================
	if((ClientSocket = socket(AF_INET,SOCK_STREAM,0))<0){
			perror("Socket error: "); return 0;}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(Port);
	serverAddr.sin_addr.s_addr= inet_addr(argv[1]); // IP_Address;
	if(connect(ClientSocket,(struct sockaddr *)&serverAddr,sizeof(serverAddr))<0){
		perror("Connection unsuccessful: "); return 0;}


	if(send(ClientSocket,buffer,BUFFER_SIZE,0)<0){
		perror("Transmission unsuccessful: "); return 0;}

	// Receive messages - while ->
	//	 	select(socket + stdin)
	/*		if timeout -> check connection to server
	 * 		if socket jumped -> read message
	 * 		if stdin jumped -> exit program
	 */

	//Initialize file descriptor set
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(ClientSocket,&fdset);
	FD_SET(fileno(stdin), &fdset);

	char name[6],msg[BUFFER_SIZE];

	int p;
	fd_set fdcopy=fdset;
	while(1){
		fdset=fdcopy;
		p=select(FD_SETSIZE,&fdset, NULL, NULL, NULL);
		memset(msg,255,0); memset(buffer,255,0);
		if(p==-1){
			perror("Select error:"); shutdown(ClientSocket,SHUT_RDWR); return 0;
		}

		if(FD_ISSET(ClientSocket,&fdset)){
			if(recv(ClientSocket,buffer,BUFFER_SIZE,0)<1){
				puts("Server closed, exiting... \n"); break;
			}
			else{
				memset(msg,255,0);
				memcpy(name,buffer,6);
				memcpy(msg,buffer+8,255);
				//User: <user_name>: has said: <the message>
				printf("User: <%s>: has said: %s \n",name,msg);
			}
			FD_CLR(ClientSocket,&fdset);
		}

		if(FD_ISSET(fileno(stdin),&fdset)){

			fgets(msg, 255 , stdin);
			if(!strcmp(msg,"\n")){
			puts("Goodbye... "); shutdown(ClientSocket,SHUT_RDWR); return 0;
			}
			else{
				memcpy(buffer,argv[3],6);
				*((unsigned long*)(buffer+6)) = (unsigned long)strlen(buffer);
				memcpy(buffer+8,msg,255);
				send(ClientSocket,buffer,BUFFER_SIZE,0);
			}
		}
	}
	close(ClientSocket);
	return 0;
}




