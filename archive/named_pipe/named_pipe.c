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

#define FIFO_NAME "/tmp/named_fifo"
const mode_t FIFO_MODE = 0660;

void *job(int length_msg)
{
	int fdread;
	int LENGTH_MSG = length_msg;
	if ((fdread = open(FIFO_NAME, O_RDONLY)) == -1)
	{
		fprintf(stderr, "Impossible to open the pipe: %s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	char buffer[LENGTH_MSG];
	read(fdread, buffer, LENGTH_MSG);
	printf("The message is : %s", buffer);
	pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	int LENGTH_MSG = atoi(argv[1]);
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
		job(LENGTH_MSG);
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
		write(fdwrite, message, sizeof(message));
		return EXIT_SUCCESS;
	}
}