#define BUFFER_SIZE 2000

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]) {

	short Port; // port
	int num_parts; //
	int welcomeSocket;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in welcomeAddr;
	struct sockaddr_in clientAddr;
	socklen_t addr_size;
	socklen_t clientAddr_size;

	// IP Address - argv[1]
	Port=atoi(argv[2]); // Port
	num_parts=atoi(argv[3]);

	// Set up socket, Bind
	if((welcomeSocket = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("Socket error: "); return 0;}
	welcomeAddr.sin_family = AF_INET;
	welcomeAddr.sin_port = htons(Port);
	welcomeAddr.sin_addr.s_addr=inet_addr(argv[1]);

	addr_size = sizeof(welcomeAddr);
	if(bind(welcomeSocket,(struct sockaddr *)&welcomeAddr,addr_size)<0){
		perror("Bind error: "); return 0;}

	// Wait for connection
	int acceptSocket,listenEntry;
	listenEntry=listen(welcomeSocket,1);
	acceptSocket=accept(welcomeSocket,(struct sockaddr *)&clientAddr,&clientAddr_size);
	if(listenEntry==-1 || acceptSocket==-1){
		perror("Error occurred in listen/accept: ");
		return 0;
	}

	// Open File
	FILE *fp;
	fp = fopen(argv[4],"r+");
	if (fp==NULL) {puts("File error."); exit (1);}

	int BytesSent=0,sumBytes=0;
	while(sumBytes < num_parts*BUFFER_SIZE){
		fread(buffer,BUFFER_SIZE,1,fp);
		BytesSent=send(acceptSocket,buffer,BUFFER_SIZE,0);
		if(BytesSent==-1){puts("Sending error."); break;}
		sumBytes+=BytesSent;
	}
	printf("Number of bytes sent: %d \n",sumBytes);
	close(welcomeSocket); close(acceptSocket);
	return 0;
}

