#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

//get fifo name and mode
#define FIFO_NAME "/tmp/named_fifo"
const mode_t FIFO_MODE = 0660;

int main(int argc, char *argv[])
{
    //parse arguments
    int LENGTH_MSG = atoi(argv[1]);
    int number_of_sends = 0;
    int last_send = LENGTH_MSG;
    clock_t begin = clock();

    //separate the message in 10KB messages
    if (LENGTH_MSG > 10000)
    {
        number_of_sends = LENGTH_MSG / 10000;
        //last message is the remaining part (less than 10KB)
        last_send = LENGTH_MSG % 10000;
        LENGTH_MSG = 10000;
    }

    //create new fifo
    if (mkfifo(FIFO_NAME, FIFO_MODE) == -1)
    {
        printf("Destroying already existing fifo: %s\n", FIFO_NAME);
        unlink(FIFO_NAME);
        if (mkfifo(FIFO_NAME, FIFO_MODE) == -1)
        {
            printf("Could not create fifo: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    //start the reader
    pid_t child = fork();
    char actualpath[100];
    if (child == -1)
    {
        //Error handling
        perror("fork");
        return EXIT_FAILURE;
    }
    if (child == 0)
    {
        char *realpath_2 = realpath("../named_pipe/named_pipe_client", actualpath);
        execlp(realpath_2, realpath_2, argv[1], argv[2], NULL);
    }
    else
    {
        //open fifo
        int fdwrite;
        if ((fdwrite = open(FIFO_NAME, O_WRONLY)) == -1)
        {
            printf("Cannot open fifo: %s\n",
                   strerror(errno));
            exit(EXIT_FAILURE);
        }

        //fill message with random chars
        char *message = NULL;
        char *last_message = NULL;
        int i, n, rnd;
        if (number_of_sends != 0)
        {
            message = malloc(LENGTH_MSG);
            for (int j = 0; j < number_of_sends; j++)
            {
                srand(time(NULL));
                if (number_of_sends != 0)
                {

                    for (i = 0; i < LENGTH_MSG - 1; ++i)
                    {
                        rnd = rand();
                        n = (rnd >> 4) & 0xF;
                        *(message + i) = (rnd & 0xF) & 1
                                             ? (n % 10) + '0'
                                             : (n % 26) + 'A';
                    }
                    message[LENGTH_MSG - 1] = 0;
                }
                //send message
                write(fdwrite, message, LENGTH_MSG);
            }
            free(message);
        }

        //fill and send last message
        if (last_send != 0)
        {
            last_message = malloc(last_send);
            for (i = 0; i < last_send - 1; ++i)
            {
                rnd = rand();
                n = (rnd >> 4) & 0xF;
                *(last_message + i) = (rnd & 0xF) & 1
                                          ? (n % 10) + '0'
                                          : (n % 26) + 'A';
            }
            last_message[last_send - 1] = 0;
            write(fdwrite, last_message, last_send);
            free(last_message);
        }
        
        //end clock and return exec time
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("sent %iMB in %f seconds\n", atoi(argv[1]) / 1000000, time_spent);
        return EXIT_SUCCESS;
    }
}