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

void *job(int number_sends,int last_send, int length_msg)
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
            printf("data %s\n", buffer);
            }
        }
        if (last_send!= 0){
       	read(fdread, buffer_last, last_send);
            printf("last data %s\n", buffer_last);
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
	if (mkfifo(FIFO_NAME, FIFO_MODE) == -1)
	{
		printf("Destroying already existing fifo: %s\n", FIFO_NAME);
		unlink(FIFO_NAME);
	}
	if (mkfifo(FIFO_NAME, FIFO_MODE) == -1)
	{
		printf("Could not create fifo: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	pid_t child = fork();
	if (child == -1)
	{
		printf("Problem fork\n");
		exit(EXIT_FAILURE);
	}
	if (child == 0)
	{
		job(number_of_sends,last_send, LENGTH_MSG);
	}
	else
	{
		int fdwrite;
		if ((fdwrite = open(FIFO_NAME, O_WRONLY)) == -1)
		{
			printf("Cannot open fifo: %s\n",
				   strerror(errno));
			exit(EXIT_FAILURE);
		}
		 char* message = NULL;
        char* last_message = NULL;
        int i, n, rnd;
		if(number_of_sends != 0){
        for (int j = 0; j < number_of_sends; j++)
        {
            message = malloc(LENGTH_MSG);
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

            write(fdwrite, message, LENGTH_MSG);
            usleep(20000);
            free(message);
        }
		}
        if (last_send != 0){
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
		
		return EXIT_SUCCESS;
	}
}