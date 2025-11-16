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
	int ReceiverSocket;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in ReceiverAddr;
	struct sockaddr_in senderAddr;
	socklen_t addr_size;

	//in_addr_t IP_Address=inet_network(argv[1]); // IP Address
	Port=atoi(argv[2]); // Port
	num_parts=atoi(argv[3]);

	// Set up socket, Bind
	if((ReceiverSocket = socket(AF_INET,SOCK_DGRAM,0))<0){
		perror("Socket error: "); return 0;}
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(Port);
	ReceiverAddr.sin_addr.s_addr=inet_addr(argv[1]);

	senderAddr.sin_family = AF_INET;
	senderAddr.sin_port = htons(Port);
	senderAddr.sin_addr.s_addr=inet_addr("192.6.1.1");
	socklen_t senderAddrSize = sizeof(senderAddr);

	addr_size = sizeof(ReceiverAddr);
	if(bind(ReceiverSocket,(struct sockaddr *)&ReceiverAddr,addr_size)){
		perror("Bind error: "); return 0;}

	int BytesSent=0,sumBytes=0,blockCounter=0;
	while(blockCounter < num_parts){
		BytesSent=recvfrom(ReceiverSocket,buffer,BUFFER_SIZE,0,(struct sockaddr *)&senderAddr,&senderAddrSize);
		if(BytesSent > 0) blockCounter++;
		if(BytesSent==-1) {puts("Receive error."); break;}
		printf("%s \n",buffer);
		sumBytes+=BytesSent;
	}
	int tmp = BUFFER_SIZE * num_parts;
	printf("Expected bytes: %d \n",tmp);
	printf("Bytes Received: %d \n",sumBytes);
	return 0;
}

