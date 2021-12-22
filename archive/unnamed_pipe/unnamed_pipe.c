#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

void job(int *tube,int number_sends, int last_send, int LENGTH_MSG)
{
    char message[LENGTH_MSG]; 
    char last_message[last_send]; 
    int tid = getpid();
    // timer pour attendre maximum 5 secondes
    int i = 5;
    while (i > 0)
    {
        if(number_sends != 0){
        for(int i=0; i < number_sends; i++){
            // lecture dans le tube
            if (read(*tube, message, LENGTH_MSG) > 0)
            {
                printf("data %s\n", message);
            }
            usleep(100);
        }
        }
        if (read(*tube, last_message, last_send) > 0)
            {
                printf("last data %s\n", last_message);
                break;
            }
        //sleep(1);
    }
}

int main(int argc, char *argv[])
{
    int LENGTH_MSG = atoi(argv[1]);
    int number_of_sends = 0;
    int last_send = LENGTH_MSG;
    if (LENGTH_MSG > 10000){
        number_of_sends = LENGTH_MSG/10000;
        last_send = LENGTH_MSG%10000;
        LENGTH_MSG = 10000;
        //printf("%i, %i, %i \n", number_of_sends, last_send, LENGTH_MSG);
    }
    char message[number_of_sends][LENGTH_MSG];
    char last_message[last_send];
    int i, n, rnd;
    srand(time(NULL));
    if (number_of_sends != 0){
    for (int j = 0; j < number_of_sends; j++){
   
    
        for (i = 0; i < LENGTH_MSG - 1; ++i){
            rnd = rand();
            n = (rnd >> 4) & 0xF;
            *(message[j] + i) = (rnd & 0xF) & 1
                             ? (n % 10) + '0'
                             : (n % 26) + 'A';
        }
    message[j][LENGTH_MSG - 1] = 0;
    }
    }
    for (i = 0; i < last_send - 1; ++i){
            rnd = rand();
            n = (rnd >> 4) & 0xF;
            *(last_message + i) = (rnd & 0xF) & 1
                             ? (n % 10) + '0'
                             : (n % 26) + 'A';
    }
    last_message[last_send - 1] = 0;
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
        job(&tube[0], number_of_sends, last_send, LENGTH_MSG);
        close(tube[0]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        for (int j = 0; j < number_of_sends; j++){
        write(tube[1], message[j], LENGTH_MSG);
        usleep(5000);
        }
        write(tube[1], last_message, last_send);
        close(tube[0]);
        close(tube[1]);
    }
}
