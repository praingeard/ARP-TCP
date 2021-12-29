#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

void job(int *tube, int number_sends, int last_send, int LENGTH_MSG)
{
    //start the message
    char message[LENGTH_MSG];
    char last_message[last_send];
    if (number_sends != 0)
    {
        for (int i = 0; i < number_sends; i++)
        {
            // read the tube
            read(*tube, message, LENGTH_MSG);
            //printf("%s\n", message);
            usleep(100);
        }
    }
    //last send is only used if the last message is shorter than the other ones
    if (last_send != 0)
    {
        read(*tube, last_message, last_send);
    }
}

int main(int argc, char *argv[])
{
    //init clocks
    clock_t begin;
    clock_t end;
    int LENGTH_MSG = atoi(argv[1]);
    int number_of_sends = 0;
    int last_send = LENGTH_MSG;

    //cut the data into groups of 10KB to avoid overflowing the pipe buffer
    //last message is the remainder (if less than 10KB)
    if (LENGTH_MSG > 10000)
    {
        number_of_sends = LENGTH_MSG / 10000;
        last_send = LENGTH_MSG % 10000;
        LENGTH_MSG = 10000;
    }

    //initialize tube and fork
    int tube[2];
    pipe(tube);
    begin = clock();
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    else if (pid == 0)
    {
        //start reader
        close(tube[1]);
        job(&tube[0], number_of_sends, last_send, LENGTH_MSG);
        close(tube[0]);
    }
    else
    {
        //fill message array with random data
        char *message = NULL;
        char *last_message = NULL;
        int i, n, rnd;
        close(tube[0]);
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
            //write in the tube
            write(tube[1], message, LENGTH_MSG);
        }
        free(message);

        //fill the last message with random data
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
            //send last message
            write(tube[1], last_message, last_send);
            free(last_message);
        }
        //close the tube and send execution time
        close(tube[1]);
        end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("sent %iMB in %f seconds\n", atoi(argv[1]) / 1000000, time_spent);
    }
}
