#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int length_message = atoi(argv[1]);
	int number_of_sends = 0;
	int last_send = length_message;
	if (length_message > 1000000)
	{
		number_of_sends = length_message / 1000000;
		last_send = length_message % 1000000;
		length_message = 1000000;
		//printf("%i, %i, %i \n", number_of_sends, last_send, length_message);
	}
	int socket_desc, client_sock, c, read_size;
	struct sockaddr_in server, client;
	char client_message[2 * length_message];

	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Server: Could not create socket");
	}
	puts("Server: Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8890);

	//Bind
	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		//print the error message
		perror("Server: bind failed. Error");
		return 1;
	}

	//Listen
	listen(socket_desc, 3);

	//Accept and incoming connection
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
	if (client_sock < 0)
	{
		perror("Server: accept failed");
		return 1;
	}
	puts("Server: client connected");
	while ((read_size = recv(client_sock, client_message, 100000000, 0)) > 0)
	{
		//Send the message back to client
		write(client_sock, client_message, strlen(client_message));
	}
	puts("Server: terminated");

	if (read_size == 0)
	{
		puts("Server: Client disconnected");
		fflush(stdout);
	}
	close(client_sock);

	return 0;
}