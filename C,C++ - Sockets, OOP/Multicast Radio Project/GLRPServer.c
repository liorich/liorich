/*
 * GLRPClient.c
 *
 *  Created on: Dec 25, 2021
 *      Authors:
 *      	Lior Ichiely
 *      	Guy Raveh
 */

#define BUFFER_SIZE 1000
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

struct Node{
	int SocketNum;
	struct sockaddr_in SocketAddress;
	int TableIndex;
	socklen_t SocketSize;
	char name[20];
	struct Node* Next;
};

void getServerIP(char *String, struct sockaddr_in * ServerAddress);
int AddNewNode(int WelcomeSocket, struct Node ** Head, fd_set * All_input);
struct Node * CheckNodes(struct Node * Head, fd_set * Ready_input, fd_set * All_input,char * buffer,char RadioTable[10][50]);
struct Node * RemoveNode(struct Node * Head, struct Node * RMV_Node, fd_set * All_input,char RadioTable[10][50]);
void initializeRadioTable(char RadioTable[10][50]);
void SendHello(int newSocket, char RadioTable[10][50], char * buffer);
struct Node * HandleNode(struct Node * PTR, char RadioTable[10][50], char * buffer,struct Node * Head,fd_set * All_input);
void SendNewStationInfo(struct Node * Head,char * Name,char * buffer, int index,int NewSocket);
void exitOrderly(struct Node * Head,struct sockaddr_in * WelcomeAddress,int WelcomeSocket);

int * ignoreFlag; // For showing timeout behavious

int main (int argc, char *argv[]){
	// Radio list table:
	char RadioTable[10][50];
	initializeRadioTable(RadioTable);

	// Start up the server.
	ignoreFlag=(int *)malloc(sizeof(int));
	*ignoreFlag=0;
	
	char buffer[BUFFER_SIZE]={0};
	int WelcomeSocket,Err;
	if((WelcomeSocket = socket(AF_INET,SOCK_STREAM,0))<0){
			printf("Error in socket(), Exiting... \n"); return 0;}
	//int NumOfListeners=0;
		//Initialize File Descriptors
		fd_set Ready_input,All_input;
		FD_ZERO(&All_input);
		FD_SET(WelcomeSocket,&All_input);
		FD_SET(fileno(stdin),&All_input);

	char ServerIP[16];
	struct sockaddr_in * WelcomeAddress=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	struct Node * Head=NULL;
	getServerIP(ServerIP, WelcomeAddress);
	while(bind(WelcomeSocket,(struct sockaddr*)WelcomeAddress,sizeof(struct sockaddr_in))<0){
		printf("Error in bind(), Please try again. Enter q to exit. \n");
		getServerIP(ServerIP, WelcomeAddress);
	}
	if(listen(WelcomeSocket,1)<0){
		printf("Error in listen(), Exiting...\n"); return 0;
	}

	// Welcome socket initialized.
	int New;
	
	// Timeout - in case no one talks over 5 minutes.
	struct timeval timeout = {300, 0};
	
	printf("Server is online! At any time, enter q to terminate. \n");
	while(1){
		Ready_input=All_input;
		Err=select(FD_SETSIZE,&Ready_input,NULL,NULL,&timeout);
		timeout.tv_sec = 600; // Refresh timeout if select jumped.
		if(Err==-1){
			printf("Error in select(). Exiting... \n");
			exitOrderly(Head,WelcomeAddress,WelcomeSocket); 
		}
		if(Err==0){
			// Timeout reached.
			printf("Timeout reached. Server now closing...");
			exitOrderly(Head,WelcomeAddress,WelcomeSocket); 
		}
		if(FD_ISSET(fileno(stdin),&Ready_input)){
			// Terminal command
			fgets(buffer, 50, stdin);
			if(buffer[0]=='q'){ // Terminate program.
				exitOrderly(Head,WelcomeAddress,WelcomeSocket); 
			}
			if(buffer[0]=='i'){ // Terminate program.
				*ignoreFlag=!(*ignoreFlag);
				printf("Ignore flag flippled. Value now: %d \n",*ignoreFlag);
			}
		}
		if(FD_ISSET(WelcomeSocket,&Ready_input)){
			printf("Incoming new connection. \n");
			// Incoming new connection.
			// Try to accept the connection:
			New=AddNewNode(WelcomeSocket,&Head,&All_input);
			
			// Returns -1 if failed, or the new socket number if succeeded.
			if(New==-1) { 
				printf("Accept returned -1, Exiting... \n"); 
				exitOrderly(Head,WelcomeAddress,WelcomeSocket); 
			}
			else SendHello(New,RadioTable,buffer); 
		}
		// And then, check all existing connection for new messages.
		Head=CheckNodes(Head,&Ready_input,&All_input,buffer,RadioTable); //
				

	}
	exitOrderly(Head,WelcomeAddress,WelcomeSocket); 

	return 0;
}

void getServerIP(char *String, struct sockaddr_in * ServerAddress){
	printf("Enter this server's IP (enter q to exit): ");
	fgets(String, sizeof(char)*16, stdin);
	if(String[0]=='q'){
		printf("Exiting... \n"); exit(EXIT_SUCCESS);
	}
	ServerAddress->sin_family = AF_INET;
	ServerAddress->sin_port = 2021;
	ServerAddress->sin_addr.s_addr = inet_addr(String);
}


// =================================================
// Linked list handling
int AddNewNode(int WelcomeSocket, struct Node ** Head, fd_set * All_input){
	struct Node * PTR=(*Head);
	if(PTR==NULL){
		// Init Head
		//printf("Allocating head. \n");
		PTR = (struct Node *)malloc(sizeof(struct Node));
		PTR->SocketNum=accept(WelcomeSocket,(struct sockaddr *)&(PTR->SocketAddress),&(PTR->SocketSize));
		if(PTR->SocketNum<0){ // Accept failed.
			free(PTR); return -1;
		}
		PTR->Next=NULL;
		PTR->TableIndex=-1;
		FD_SET(PTR->SocketNum,All_input);
		printf("Client %d has connected. \n",PTR->SocketNum);
		(*Head)=PTR;
		return PTR->SocketNum;
	}
	else{ // Run until end
		while(PTR->Next!=NULL) PTR=PTR->Next;

		PTR->Next=(struct Node *)malloc(sizeof(struct Node));
		PTR=PTR->Next;
		PTR->SocketNum=accept(WelcomeSocket,(struct sockaddr *)&(PTR->SocketAddress),&(PTR->SocketSize));
		if(PTR->SocketNum<0){ // Accept failed.
				free(PTR); PTR=NULL;
				return -1;			
		}
		printf("Client %d has connected. \n",PTR->SocketNum);
		PTR->Next=NULL;
		PTR->TableIndex=-1;
		FD_SET(PTR->SocketNum,All_input);
		return PTR->SocketNum;
	}
}

struct Node * CheckNodes(struct Node * Head, fd_set * Ready_input, fd_set * All_input,char * buffer,char RadioTable[10][50]){
	// Check list for any new requests/TCP disconnections.
	
	struct Node * PTR=Head;
	if(Head==NULL) { printf("Head was null \n"); return NULL; }
	while(PTR!=NULL){
		if(FD_ISSET(PTR->SocketNum,Ready_input)){
			if(recv(PTR->SocketNum,buffer,BUFFER_SIZE,0)<1){
				//TCP Disconnection, Remove node
				Head=RemoveNode(Head,PTR,All_input,RadioTable);
				return Head;
			}
			else{
				if(*ignoreFlag==1); // If ignore flag is up, don't reply to any queries.
				else Head=HandleNode(PTR,RadioTable,buffer,Head,All_input);
			}
		}
		PTR=PTR->Next;
	}
	return Head;
}

struct Node * RemoveNode(struct Node * Head, struct Node * RMV_Node, fd_set * All_input,char RadioTable[10][50]){
	// Removes a node.
	struct Node * PTR=Head;
	printf("Client %d has disconnected. \n",RMV_Node->SocketNum);
	if(RMV_Node->SocketNum == PTR->SocketNum){
		// Remove the head.
		Head=Head->Next;
		FD_CLR(PTR->SocketNum,All_input); close(PTR->SocketNum);
		if(PTR->TableIndex!=-1){
			printf("Freed index: %d \n",PTR->TableIndex);
			RadioTable[PTR->TableIndex][0]='0';
			memset(RadioTable[PTR->TableIndex]+16,0,34);
		}
		//printf("Removed a head node. \n");
		free(PTR);
		return Head;
	}
	else{
		// Remove someone in the middle of the list.
		while(PTR->Next->SocketNum != RMV_Node->SocketNum)
			PTR=PTR->Next;
		PTR->Next=PTR->Next->Next;
		FD_CLR(RMV_Node->SocketNum,All_input); close(RMV_Node->SocketNum);
		if(RMV_Node->TableIndex!=-1){
			printf("Freed index: %d \n",RMV_Node->TableIndex);
			RadioTable[RMV_Node->TableIndex][0]='0';
			memset(RadioTable[RMV_Node->TableIndex]+16,0,34);
		}
		//printf("Removed not a head node. \n");
		free(RMV_Node);
		return Head;
	}
}
// ===================================================

//Node handling: Hello message, ...
void SendHello(int newSocket,char RadioTable[10][50], char * buffer){
	char index[2]={0};
	memset(buffer,0,BUFFER_SIZE);
	strcpy(buffer,"H");
	strcat(buffer,"Hello! Here are the list of current streamers: \n");
	int i=0;
	for(i=0;i<10;i++){
		if(RadioTable[i][0]=='0') continue;
		index[0]=RadioTable[i][9];
		strcat(buffer,index);
		strcat(buffer,")");
		strcat(buffer,RadioTable[i]+16);
		//strcat(buffer,"\n");
	}
	send(newSocket,buffer,BUFFER_SIZE,0);
}	

struct Node * HandleNode(struct Node * PTR,char RadioTable[10][50], char * buffer,struct Node * Head,fd_set * All_input){
	int Socket=PTR->SocketNum;
	// Handles a node according to the message.
	char Name[30]={0};
	int index;
	
	// For class, display the contents of the message.
	printf("Client with socket number %d has sent a message of type %c. \n",Socket,buffer[0]);
	
	switch(buffer[0]){
	case 'H':{ // Send a hello message to them.
		SendHello(PTR->SocketNum,RadioTable,buffer);
		return Head;
	}
	case 'J':{ // Join request -> reply with Join Confirm + Multicast IP
				// Msg example: "J2"
		index=atoi(buffer+1);
		// If a user requested to join a station that is not live, do not reply.
		if(index < 0 || index > 9) return Head;
		if(RadioTable[index][0]=='0') return Head;
		// Else, reply with the multicast IP in the Join Confirm message.
		memset(buffer,0,BUFFER_SIZE);
		strcat(buffer,"j");
		memcpy(buffer+1,RadioTable[index]+1,49);
		send(Socket,buffer,BUFFER_SIZE,0);
		return Head;
	}
	
	case 'T':{ // Transmit request -> reply with Transmit Confirm + Multicast IP
		
		// In case a client asked to transmit twice, forcefully close their connection.
		if(PTR->TableIndex!=-1){
			Head=RemoveNode(Head,PTR,All_input,RadioTable);
			return Head;
		}
		
		int i=0;
		strcpy(Name,buffer+1);
		memset(buffer,0,BUFFER_SIZE);
		strcat(buffer,"t");
		while(RadioTable[i][0]=='1') i++;
		if(i==10){ // All addresses are taken
			buffer[1]='N';
			send(Socket,buffer,BUFFER_SIZE,0);
			return Head;
		}
		// Update radio table
		RadioTable[i][0]='1';
		strcpy(RadioTable[i]+16,Name);
		PTR->TableIndex=i;
		
		// Copy multicast IP to the Transmit Confirm message, and send
		memcpy(buffer+1,RadioTable[i]+1,16);
		send(Socket,buffer,BUFFER_SIZE,0);
		
		// Send new station info to everyone
		SendNewStationInfo(Head,Name,buffer,PTR->TableIndex,PTR->SocketNum);
		
		return Head;
	}
	
	
	case 'S':{ // Transmit stop request -> reply with Transmit Stop Confirm
		if(PTR->TableIndex==-1) return Head;
		RadioTable[PTR->TableIndex][0]='0';
		memset(RadioTable[PTR->TableIndex]+16,0,34);
		PTR->TableIndex=-1;
		memset(buffer,0,BUFFER_SIZE);
		strcat(buffer,"s");
		send(Socket,buffer,BUFFER_SIZE,0);
		return Head;
		}
	}
	return Head;
}

void SendNewStationInfo(struct Node * Head, char * Name, char * buffer, int index, int NewSocket){
	// N message - New radio station.
	memset(buffer,0,BUFFER_SIZE);
	buffer[0]='N';
	strcpy(buffer+1,Name);
	strcat(buffer," has started to stream. Enter /j ");
	char indx[2]="\0\0"; indx[0]=index+'0';
	strcat(buffer, indx);
	strcat(buffer," to join them. \n");
	
	struct Node * PTR=Head;
	while(PTR!=NULL){
		if(PTR->SocketNum==NewSocket); // Do nothing - Don't print the message to yourself.
		else send(PTR->SocketNum,buffer,BUFFER_SIZE,0);
		PTR=PTR->Next;
	}
}

void initializeRadioTable(char RadioTable[10][50]){
	// Statically initializes the radio table to include the multicast IPs.
	strcpy(RadioTable[0],"0224.6.0.0\0\0\0\0\0\0");
	strcpy(RadioTable[1],"0224.6.0.1\0\0\0\0\0\0");
	strcpy(RadioTable[2],"0224.6.0.2\0\0\0\0\0\0");
	strcpy(RadioTable[3],"0224.6.0.3\0\0\0\0\0\0");
	strcpy(RadioTable[4],"0224.6.0.4\0\0\0\0\0\0");
	strcpy(RadioTable[5],"0224.6.0.5\0\0\0\0\0\0");
	strcpy(RadioTable[6],"0224.6.0.6\0\0\0\0\0\0");
	strcpy(RadioTable[7],"0224.6.0.7\0\0\0\0\0\0");
	strcpy(RadioTable[8],"0224.6.0.8\0\0\0\0\0\0");
	strcpy(RadioTable[9],"0224.6.0.9\0\0\0\0\0\0");
}

void exitOrderly(struct Node * Head,struct sockaddr_in * WelcomeAddress,int WelcomeSocket){
	//Orderly terminates the process.
	struct Node * PTR=Head;
	while(Head!=NULL){ // Free list and close sockets.
		Head=Head->Next;
		close(PTR->SocketNum);
		free(PTR);
		PTR=Head;
	}
	free(WelcomeAddress);
	close(WelcomeSocket);
	free(ignoreFlag);
	printf("Goodbye... \n");
	exit(EXIT_SUCCESS);
	
}










