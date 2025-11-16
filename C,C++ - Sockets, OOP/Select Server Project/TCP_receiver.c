#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2000

int main(int argc, char *argv[]) {

	short Port; // port
	int num_parts; //
	int ReceiverSocket;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	// IP Address - argv[1]
	Port=atoi(argv[2]); // Port
	num_parts=atoi(argv[3]); // num_parts

	// Set up socket, Connect
	if((ReceiverSocket = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("Socket error: "); return 0;}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(Port);
	serverAddr.sin_addr.s_addr= inet_addr(argv[1]); // IP_Address;
	addr_size= sizeof(serverAddr);
	if(connect(ReceiverSocket,(struct sockaddr *)&serverAddr,addr_size)<0){
		perror("Connection unsuccessful: "); return 0;}


	// Requesting data
	printf("Requesting data... \n");
	int BytesReceived=0,sumBytes=0;
	while(sumBytes < num_parts*BUFFER_SIZE){
		BytesReceived = recv(ReceiverSocket,buffer,BUFFER_SIZE,0);
		if(BytesReceived == 0 || BytesReceived == -1){puts("Receive error."); break;}
		sumBytes+=BytesReceived;
		printf("%s \n",buffer);
	}
	int tmp = BUFFER_SIZE * num_parts;
	printf("Expected bytes: %d \n",tmp);
	printf("Bytes received: %d \n",sumBytes);
	close(ReceiverSocket);
	return 0;
}
