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
	int SenderSocket;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in SenderAddr;
	struct sockaddr_in ReceiverAddr;
	socklen_t addr_size;

	//in_addr_t IP_Address=inet_network(argv[1]); // IP Address
	Port=atoi(argv[2]); // Port
	num_parts=atoi(argv[3]);

	// Set up socket, Bind
	if((SenderSocket = socket(AF_INET,SOCK_DGRAM,0)) < 0){
		perror("Socket error: "); return 0;}
	SenderAddr.sin_family = AF_INET;
	SenderAddr.sin_port = htons(Port);
	SenderAddr.sin_addr.s_addr=inet_addr(argv[1]);

	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(Port);
	ReceiverAddr.sin_addr.s_addr=inet_addr("192.6.2.1");

	addr_size = sizeof(SenderAddr);
	if(bind(SenderSocket,(struct sockaddr *)&SenderAddr,addr_size)<0){
		perror("Bind error: "); return 0;}

	// Open File
	FILE *fp;
	fp = fopen(argv[4],"r+");
	if (fp==NULL) {puts("File error."); exit (1);}

	int BytesSent=0,sumBytes=0,blockCounter=0;
	while(blockCounter < num_parts){
		fread(buffer,BUFFER_SIZE,1,fp);
		BytesSent=sendto(SenderSocket,buffer,BUFFER_SIZE,0,(struct sockaddr *)&ReceiverAddr,sizeof(ReceiverAddr));
		blockCounter++;
		if(BytesSent==-1){puts("Sending error."); break;}
		sumBytes+=BytesSent;
	}
	printf("Number of bytes sent: %d \n",sumBytes);
	return 0;
}


