/*
	C ECHO client example using sockets
*/
#include <stdio.h>      //printf
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int LENGTH_MSG = atoi(argv[1]);

    char server_reply[2*LENGTH_MSG];
    int sock;
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        fflush(stdout);
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8890);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    //keep communicating with server
    while (1)
    {
        char message[LENGTH_MSG];
        int i, n, rnd;
        srand(time(NULL));
        for (i = 0; i < LENGTH_MSG - 1; ++i)
        {
            rnd = rand();
            n = (rnd >> 4) & 0xF;
            *(message + i) = (rnd & 0xF) & 1
                                 ? (n % 10) + '0'
                                 : (n % 26) + 'A';
        }
        message[LENGTH_MSG - 1] = 0;

        //Send some data
        if (send(sock, message, strlen(message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

        //Receive a reply from the server
        if (recv(sock, server_reply, 2000, 0) < 0)
        {
            puts("recv failed");
            break;
        }

        puts("Server reply :");
        puts(server_reply);
    }
    close(sock);
    return 0;
}