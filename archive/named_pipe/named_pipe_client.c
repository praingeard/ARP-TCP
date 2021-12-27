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

#define FIFO_NAME "/tmp/named_fifo"
const mode_t FIFO_MODE = 0660;

void *job(int number_sends, int last_send, int length_msg)
{

    int fdread;
    if ((fdread = open(FIFO_NAME, O_RDONLY)) == -1)
    {
        fprintf(stderr, "Impossible to open the pipe: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    char buffer[length_msg];
    char buffer_last[last_send];
    if (number_sends != 0)
    {
        for (int i = 0; i < number_sends; i++)
        {
            // lecture dans le tube
            read(fdread, buffer, length_msg);
        }
    }
    if (last_send != 0)
    {
        read(fdread, buffer_last, last_send);
    }
}

int main(int argc, char *argv[])
{
     printf("named_pipe_client started with arg %i\n", atoi(argv[1]));
    int LENGTH_MSG = atoi(argv[1]);
    int number_of_sends = 0;
    int last_send = LENGTH_MSG;
    if (LENGTH_MSG > 10000)
    {
        number_of_sends = LENGTH_MSG / 10000;
        last_send = LENGTH_MSG % 10000;
        LENGTH_MSG = 10000;
        //printf("%i, %i, %i \n", number_of_sends, last_send, LENGTH_MSG);
    }
    clock_t begin = clock();
    job(number_of_sends, last_send, LENGTH_MSG);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("sent %iMB in %f seconds\n", atoi(argv[1]) / 1000000, time_spent);
    return EXIT_SUCCESS;
}