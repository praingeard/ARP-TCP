#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define LENGTH_MSG 30

char message[LENGTH_MSG] = "";

void job(int *tube)
{
    int tid = getpid();
    // timer pour attendre maximum 5 secondes
    int i = 5;
    while (i > 0)
    {
        // lecture dans le tube
        if (read(*tube, message, LENGTH_MSG) > 0)
        {
            printf("Message du processus [%i] : %s", tid, message);
            break;
        }
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
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
    puts(message);
    int tube[2];
    pipe(tube);
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    else if (pid == 0)
    {
        close(tube[1]);
        job(&tube[0]);
        close(tube[0]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        write(tube[1], message, LENGTH_MSG);
        close(tube[0]);
        close(tube[1]);
    }
}
