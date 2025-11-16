/*
 * GLRPClient.c
 *
 *  Created on: Dec 25, 2021
 *      Authors:
 *      	Lior Ichiely
 *      	Guy Raveh
 */

#define BUFFER_SIZE 1000
#define UPLOAD_NANOSLEEP_IN_US 58500000
#define STREAM_NANOSLEEP_IN_US 62500
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
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

void getServerIP(char *String, struct sockaddr_in * ServerAddress);
void exitOrderly(int Socket,struct sockaddr_in * ServerAddress,pthread_t Transmitter,pthread_t Listener);
void PrintMenu();
void * Transmit();
void * Listen();



// Thread B: Listening to a multicast group
/* The main thread requests and receives the multicast IP which is stored in the Listen_IP.
 * The flag ListenFlag is used to maintain its state and handle errors.
 */
int * ListenFlag;
char * Listen_IP;
void * Listen(){
	struct ip_mreq mreq;
	int ListenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in ListenAddr;
	socklen_t ListenSize;
	ListenAddr.sin_family = AF_INET;
	ListenAddr.sin_port=2022;
	ListenAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(ListenSocket,(struct sockaddr*)&ListenAddr,sizeof(ListenAddr))<0){
		printf("Could not join the multicast radio. Terminating Listener. \n");
		*ListenFlag=0;
		pthread_exit(NULL);
	}
	mreq.imr_multiaddr.s_addr=inet_addr(Listen_IP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	// IP Add membership message:
	setsockopt(ListenSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	FILE *pipeout;
	char buf[1000] = {0};
	struct timespec req,rem;
	req.tv_sec=0; req.tv_nsec=STREAM_NANOSLEEP_IN_US;
	pipeout = popen("play -t mp3 -> /dev/null 2>&1","w");
	if(pipeout==NULL){
		printf("Could not open file pipe. Terminating Listener. \n");
		*ListenFlag=0;
		pthread_exit(NULL);
	}
	
	// Initializing the Listener's file descriptor set.
	fd_set Ready_input,All_input;
	FD_ZERO(&All_input);
	FD_SET(ListenSocket,&All_input);
	FD_SET(fileno(stdin),&All_input);
	struct timeval timeout = {20, 0};
	int Err;
	
	while(1){
		Ready_input = All_input;
		Err=select(FD_SETSIZE,&Ready_input,NULL,NULL,&timeout);
		if(Err==-1){
			printf("Listener's select ran into an error. Exiting... \n"); 
			*ListenFlag=0;
			pthread_exit(NULL);
		}
		if(Err==0){
			printf("Listener reached timeout. Stopping listener.");
			*ListenFlag=0;
			pthread_exit(NULL);
		}
		
		if(*ListenFlag==0){ break;} // User decided to leave.
		// Check if there's new data on the UDP socket. If not, continue.
		if(!FD_ISSET(ListenSocket,&Ready_input)) continue;
		
		timeout.tv_sec = 20; // Reset the listening timeout.
		recvfrom(ListenSocket,buf,1000,MSG_DONTWAIT,(struct sockaddr*)&ListenAddr,&ListenSize);
		
		// Check if the transmitter sent a PLAY message.
		// Message body: "PLAYNow playing ABCD.mp3..."
		if(memcmp(buf,"PLAY",4)==0){
			// PLAY message was sent. Display it...
			puts(buf+4);
			memset(buf,0,BUFFER_SIZE);
			continue;
		}		
		
		// Check if the transmitter sent a CLOSE message.
		// Message body: "CLOSE\0\0\0\0\0....."
		if(memcmp(buf,"CLOSE",5)==0){
			// CLOSE message was sent. Close listener...
			printf("CLOSE message received. \n");
			break;
		}
		
		// Else, write song data.		
		nanosleep(&req,&rem);
		fwrite(buf,1000,sizeof(char),pipeout);
	}
	setsockopt(ListenSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
	pclose(pipeout);
	close(ListenSocket);
	printf("Listener terminated. \n");
	*ListenFlag=0;
	return NULL;
}




// Thread C: Transmitting to a multicast group
/* Likewise, the mainthread requests and receives a multicast IP to transmit to, stored in Transmit_IP.
 * The current song to play is maintained in SongName, and if SongName[0] == '%', this indicates there are no songs right now.
 * TransmitFlag is used to handle transmission state and errors.
 */
char * Transmit_IP;
char * SongName;
int * TransmitFlag;

void * Transmit(){
	int TransmitSocket=socket(AF_INET,SOCK_DGRAM,0);
	if (TransmitSocket<0){
		printf("Error in transmitter socket creation. Aborting... \n");
		*TransmitFlag=0;
		pthread_exit(NULL);
	}
	
	struct sockaddr_in TransmitAddr;
	TransmitAddr.sin_family=AF_INET;
	TransmitAddr.sin_port=2022;
	TransmitAddr.sin_addr.s_addr=inet_addr(Transmit_IP);
	unsigned char ttl=32;
	setsockopt(TransmitSocket, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&ttl, sizeof(ttl));
	char buf[1000] = {0};
	while(1){
		// This is used to identify if there is a song to transmit.
		// The user must use the command /p 0.mp3.
		
		// Case 1: User decided to stop streaming.
		if(*TransmitFlag==0) { 
			memset(buf,0,BUFFER_SIZE);
			strcpy(buf,"CLOSE");
			printf("Sent CLOSE message. \n");
			sendto(TransmitSocket,buf,1000,0,(struct sockaddr *)&TransmitAddr, sizeof(TransmitAddr));
			pthread_exit(NULL); }
		if(SongName[0]=='%'){ sleep(1); continue; }
		
		// Try to open the file:
		FILE *pipein;
		SongName[strlen(SongName)-1]='\0'; //file opening syntax: must remove the \n at the end of the string.
		pipein = fopen(SongName,"r");
		if(pipein==NULL) { 
			printf("Could not open this file. Command syntax example: /p ABCD.mp3 \n");
			SongName[0]='%';
			continue;
		}
		
		// Case 2: Song chosen, Send a "Now playing message" and send the song:
		memset(buf,0,BUFFER_SIZE);
		strcpy(buf,"PLAY");
		strcat(buf,"Now playing: ");
		strcat(buf,SongName);
		strcat(buf,"...");
		sendto(TransmitSocket,buf,1000,0,(struct sockaddr *)&TransmitAddr, sizeof(TransmitAddr));
		
		struct timespec req,rem;
		req.tv_sec=0; req.tv_nsec=UPLOAD_NANOSLEEP_IN_US;
		printf("Transmitting... \n");
		int check;
		while(1){
			// While loop for transmitting the song.
			// Check Transmit flag: If it is 0, send a close message.
			if(*TransmitFlag==0){
				memset(buf,0,BUFFER_SIZE);
				strcpy(buf,"CLOSE");
				printf("Sent CLOSE message. \n");
				sendto(TransmitSocket,buf,1000,0,(struct sockaddr *)&TransmitAddr, sizeof(TransmitAddr));
				SongName[0]='%';
				pthread_exit(NULL);
			}			
			// Else, transmit the song.
			check=fread(buf,1000,sizeof(char),pipein);
			if(check==0) break; // file stop
			nanosleep(&req,&rem);
			sendto(TransmitSocket,buf,1000,0,(struct sockaddr *)&TransmitAddr, sizeof(TransmitAddr));
			
		}
		printf("Finished playing current song. Use /p to transmit the next song. \n");
		SongName[0]='%';
	}
}



int main (int argc, char *argv[]){
	// Initializations:
	// Thread identifiers:
	pthread_t Transmitter=0,Listener=0;
	Listen_IP = (char *)malloc(sizeof(char)*17);
	Transmit_IP = (char *)malloc(sizeof(char)*17);
	SongName = (char *)malloc(sizeof(char)*50); 
		memset(SongName,0,50);
		SongName[0]='%';
	ListenFlag = (int *)malloc(sizeof(int));
	TransmitFlag = (int *)malloc(sizeof(int));
	(*TransmitFlag)=0; // 0 = Transmit OFF, 1 = Transmit ON.
	(*ListenFlag)=0; // 0 = Do not listen, 1 = Listen
	
	// Buffer and main socket:
	char buffer[BUFFER_SIZE]={0}; char command;
	struct sockaddr_in * ServerAddress=(struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	int ClientSocket;
	if((ClientSocket = socket(AF_INET,SOCK_STREAM,0))<0){
		printf("Error in socket(), Exiting... \n"); 
		exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
	}
	
	// Initializing the file descriptor set:
	fd_set Ready_input,All_input;
	FD_ZERO(&All_input);
	FD_SET(ClientSocket,&All_input);
	FD_SET(fileno(stdin),&All_input);
	int Err;	// Select error flag.

	// Step 1: Ask for the server's IP. Constant Port 2021.
	char serverIP[16];
	getServerIP(serverIP,ServerAddress);
	while(connect(ClientSocket,(struct sockaddr*)ServerAddress,sizeof(struct sockaddr))<0){
		printf("Connection Unsuccessful. Please retry. \n");
		getServerIP(serverIP,ServerAddress);
	}

	// Step 2: Connected!
	// We set a timeout struct.
	
	struct timeval timeout = {300, 0};
	
	char MsgType; char Index[2]={0};
	char Name[30]={0};
	printf("You've been connected. Type /h for a list of commands. \n");
	while(1){
		Ready_input = All_input;
		Err=select(FD_SETSIZE,&Ready_input,NULL,NULL,&timeout);
		if(Err==-1){ // Select returned error.
			printf("Select returned error. Exiting... \n");
			exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
		}
		
		if(Err==0){ // We have reached a timeout.
			// If we're currently streaming or listening, ignore timeout.
			if(*TransmitFlag==1 || *ListenFlag==1){
				timeout.tv_sec = 300;
				continue;
			}
			printf("Timeout reached. Terminating program... \n");
			exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
		}
		
		if(FD_ISSET(ClientSocket,&Ready_input)){
			Err=recv(ClientSocket,buffer,BUFFER_SIZE,0);
			if(Err<1){
				// RECV Error occured.
				printf("Server closed connection. Exiting... \n");
				exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
			}
			else{
				// Server sent message, handle it.
				sscanf(buffer,"%1c",&MsgType);
				switch(MsgType){
				case 'H':{ // Hello message
					timeout.tv_sec = 300;
					puts(buffer+1);
					break;
				}
				case 'N':{ // New radio message
					timeout.tv_sec = 300;
					puts(buffer+1);
					break;
				}
				
				case 't':{ // Transmit Confirm
					// Msg example: "t244.6.0.5"
					// We create the multicast group 244.6.0.5 to transmit music over.
					timeout.tv_sec = 300;
					
					if(buffer[1]=='N'){
						printf("The server's multicast IP space is full. \n");
						continue;
					}
					
					memcpy(Transmit_IP,buffer+1,15);
					(*TransmitFlag)=1;
					if(pthread_create(&Transmitter,NULL,Transmit,NULL)!=0){
						printf("Thread creation failed. Exiting... \n");
						exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
					}
					
					printf("To play a song in the transmitter, use the /p command. For example: \n");
					printf("/p ABCD.mp3 \n");					
					break;
				}
				case 'j':{ // Join Confirm
					// Msg example: "j244.6.0.9"
					// We join the multicast group 244.6.0.9 to listen to it's music.
					timeout.tv_sec = 300;
					
					printf("Joining %s...",buffer+16);
					memcpy(Listen_IP,buffer+1,15);
					(*ListenFlag)=1;
					if(pthread_create(&Listener,NULL,Listen,NULL)!=0){
						(*ListenFlag)=0;
						printf("Thread creation failed. Exiting... \n");
						exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
					}
					break;
				}
				case 's':{ // Transmit Stop Confirm
					// Msg example: "s";
					// We will shut down the multicast radio.
					timeout.tv_sec = 300;
					
					(*TransmitFlag)=0;
					if(pthread_join(Transmitter,NULL)){
						printf("pthread_join error. Exiting... \n");
						exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
					}
					SongName[0]='%';
					Transmitter=0;
					printf("Transmitter stopped. \n");
					break;
				}
			}						
			}	
		}
		
		if(FD_ISSET(fileno(stdin),&Ready_input)){
			// New command from terminal.
			fgets(buffer, 50, stdin);
			if(buffer[0]!='/') continue; // Ignore anything not in command syntax.
			command=buffer[1];
			switch(command){
			case 'h':{
				timeout.tv_sec = 300;
				PrintMenu();
				break;
			}	
			case 'H':{ // Request list of current streamers (Hello message).
				timeout.tv_sec = 10;
				memset(buffer,0,BUFFER_SIZE);
				strcat(buffer,"H");
				send(ClientSocket,buffer,BUFFER_SIZE,0);
				break;
			}
			case 'j':{
				// Join message: send a join message to the server.
				// /j 2
				if(*ListenFlag==1){
					printf("You are currently listening to a radio station. Enter /l to leave it. \n");
					timeout.tv_sec = 300;
					break;
				}
				
				// 10 second timeout: if the server doesnt respond, terminate.
				timeout.tv_sec=10;
				
				Index[0]=buffer[3]; // Assuming input is correct
				memset(buffer,0,BUFFER_SIZE);
				strcat(buffer,"J");
				strcat(buffer,Index);
				send(ClientSocket,buffer,BUFFER_SIZE,0);
				break;
			}	
			case 't':{
				// Transmit message: send a transmit request to the server.
				// /t 100FM
				
				// 10 second timeout: if the server doesnt respond, terminate.
				timeout.tv_sec=10;
				
				strcpy(Name,buffer+3);
				memset(buffer,0,BUFFER_SIZE);
				strcat(buffer,"T");
				strcat(buffer,Name);
				send(ClientSocket,buffer,BUFFER_SIZE,0);
				break;
			}	
			case 's':{
				// Stop transmit message: send a transmit stop request to the server.
				
				// 10 second timeout: if the server doesnt respond, terminate.
				timeout.tv_sec=10;
				
				memset(buffer,0,BUFFER_SIZE);
				strcat(buffer,"S");
				send(ClientSocket,buffer,BUFFER_SIZE,0);
				break;
			}	
			case 'p':{
				timeout.tv_sec = 300;
				if(*TransmitFlag==0){
					printf("You're not streaming. Request to transmit using /t <Radio name> first. \n");
					break;
				}
				if(SongName[0]!='%'){
					// Busy
					printf("Cannot currently change song. Wait for the transmition to end... \n");
					break;
				}
				// Play a song to your transmitter.
				// from terminal: "/p 0.mp3"
				memset(SongName,0,50);
				strcpy(SongName,buffer+3);
				break;
			}
			case 'l':{
				timeout.tv_sec = 300;
				if(*ListenFlag==0){
					printf("Currently not listening. Join a radio using /j <ID>.\n To see listen of streamers, use /H. \n");
					break;
				}
				// Stop listening.
				(*ListenFlag)=0;
				if(pthread_join(Listener,NULL)){
					printf("pthread_join error. Terminating... \n");
					exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
				}
				Listener=0;
				break;
			}
			case 'q':{
				exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
				return 0;
			}
			}
		}
	}
	exitOrderly(ClientSocket,ServerAddress,Transmitter,Listener);
	return 0;
}

void getServerIP(char *String, struct sockaddr_in * ServerAddress){
	printf("Server IP (enter q to exit): ");
	fgets(String, sizeof(char)*16, stdin);
	if(String[0]=='q'){
		printf("Exiting... \n"); exit(EXIT_SUCCESS);
	}
	puts(String);
	ServerAddress->sin_family = AF_INET;
	ServerAddress->sin_port = 2021;
	ServerAddress->sin_addr.s_addr = inet_addr(String);
}

void PrintMenu(){
	printf("The command syntax is as follows:\n");
	printf("/h - Displays this menu.\n");
	printf("/H - Request current list of radio stations. \n");
	printf("/j <Radio Number> - Joins a radio.\n");
	printf("/t <Radio Name> - Creates your new transmission.\n");
	printf("/s <Radio Name> - Stops your radio transmission.\n");
	printf("/p <Radio Name> <Song Name and Directory> - Plays a song to your radio.\n");
	printf("/l - Leave radio station you're listening to. \n");
	printf("/q - Quit the program.\n");
	printf("======================== \n");
}

void exitOrderly(int ClientSocket,struct sockaddr_in * ServerAddress,pthread_t Transmitter,pthread_t Listener){
	// Orderly terminates the process.
	(*ListenFlag)=0;
	(*TransmitFlag)=0;
	printf("Goodbye...\n");
	if(Transmitter!=0){ // Close transmitter.
		if(pthread_join(Transmitter,NULL)){
			printf("pthread_join error. \n");
		}
	}
	if(Listener!=0){ // Close listener.
		if(pthread_join(Listener,NULL)){
			printf("pthread_join error. \n");
		}
	}
	close(ClientSocket); 
	free(Listen_IP); free(Transmit_IP); free(SongName); free(ListenFlag); free(ServerAddress);
	exit(EXIT_SUCCESS);
}