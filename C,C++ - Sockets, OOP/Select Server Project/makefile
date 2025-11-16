all: TCP_sender UDP_sender UDP_receiver chat_client chat_server chat_client_b chat_server_b Select_server

Select_server: Select_server.c
	gcc -o Select_server Select_server.c

TCP_sender: TCP_sender.c
	gcc -o TCP_sender TCP_sender.c

TCP_receiver: TCP_receiver.c
	gcc -o TCP_receiver TCP_receiver.c

UDP_sender: UDP_sender.c
	gcc -o UDP_sender UDP_sender.c

UDP_receiver: UDP_receiver.c
	gcc -o UDP_receiver UDP_receiver.c

chat_client: chat_client.c
	gcc -o chat_client chat_client.c

chat_server: chat_server.c
	gcc -o chat_server chat_server.c

chat_client_b: chat_client_b.c
	gcc -o chat_client_b chat_client_b.c

chat_server_b: chat_server_b.c
	gcc -o chat_server_b chat_server_b.c

