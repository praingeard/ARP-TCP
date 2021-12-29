#include <stdio.h>      
#include <string.h>     
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    //parse argument and cut messages into 1MB parts
    int LENGTH_MSG = atoi(argv[1]);
    int number_of_sends = 0;
    int last_send = LENGTH_MSG;
    if (LENGTH_MSG > 1000000)
    {
        number_of_sends = LENGTH_MSG / 1000000;
        last_send = LENGTH_MSG % 1000000;
        LENGTH_MSG = 1000000;
    }

    //start socket
    char server_reply[2 * LENGTH_MSG];
    int sock;
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Client: Could not create socket");
        fflush(stdout);
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8890);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Client: connect failed. Error");
        return 1;
    }

    //keep communicating with server
    clock_t begin = clock();
    while (number_of_sends != 0)
    {
        //fill message with random char
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

        //Send message
        if (send(sock, message, strlen(message), 0) < 0)
        {
            puts("Client: Send failed");
            return 1;
        }

        //Receive a reply from the server
        if (recv(sock, server_reply, 2 * strlen(message), 0) < 0)
        {
            puts("Client: recv failed");
            break;
        }
        number_of_sends--;
    }
    //same for the last message
    if (last_send != 0)
    {
        char last_message[last_send];
        int i, n, rnd;
        srand(time(NULL));
        for (i = 0; i < last_send - 1; ++i)
        {
            rnd = rand();
            n = (rnd >> 4) & 0xF;
            *(last_message + i) = (rnd & 0xF) & 1
                                      ? (n % 10) + '0'
                                      : (n % 26) + 'A';
        }
        last_message[last_send - 1] = 0;

        //Send some data
        if (send(sock, last_message, strlen(last_message), 0) < 0)
        {
            puts("Client: Send failed client");
            return 1;
        }
        else
        {
            puts("Client: Data sent to the server");
        }

        //Receive a reply from the server
        if (recv(sock, server_reply, 2 * strlen(last_message), 0) < 0)
        {
            puts("Client: recv failed client");
        }
        else
        {
            puts("Client: Data recieved from the server");
        }
    }
    //end clock and print exec time
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("sent %iMB in %f seconds\n", atoi(argv[1]) / 1000000, time_spent);
    return 0;
}