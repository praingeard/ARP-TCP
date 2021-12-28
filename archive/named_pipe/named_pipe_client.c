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
    close(fdread);
    int ret_value = remove(FIFO_NAME);
    if (ret_value != 0)
    {
        printf("\nremove failed for %s", FIFO_NAME);
        printf("\nerrno is %d", errno);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
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
    job(number_of_sends, last_send, LENGTH_MSG);
    return EXIT_SUCCESS;
}