#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "../logarp/logarp.h"

#define NUMBER_OF_PROCESSES_BACKGROUND 5
#define NB_FORK 2
#define MAX_NAME_SIZE 40
#define MAX_NUM_SIZE 40

//Wait for all children termination
void waitForAll()
{
    int status;
    pid_t pid;
    int n = 0;
    while (n < NB_FORK)
    {
        pid = wait(&status);
        n++;
    }
}

int main()
{
    //defining user input variables
    int selected_prog;
    int size_data;
    int size_buffer;

    //logfile initializing
    time_t reft = time(NULL);
    struct tm *timenow;
    timenow = localtime(&reft);
    char logname[40];
    strftime(logname, 40, "../logs/log_%d-%m-%Y_%H:%M:%S.txt", timenow);

    const char *logFileName = logname;
    printf("opening log file : %s \n", logFileName);
    fflush(stdout);

    //getting user inputs
    printf("Enter the desired programs : \n ");
    printf("1 : unnamed pipe \n ");
    printf("2 : named pipe \n ");
    printf("3 : socket \n ");
    printf("4 : circle buffer \n ");
    //checking that the input is what we wanted
    while (scanf("%d", &selected_prog) != 1 || selected_prog <= 0 || selected_prog > 4)
    {
        printf("incorrect input, please enter an integer between 1 and 4\n");
        while (getchar() != '\n')
            ;
    }

    //getting the data size
    printf("Enter the desired size of data to send in Megabytes (max 100MB) : \n ");
    while (scanf("%d", &size_data) != 1 || size_data <= 0 || size_data > 100)
    {
        printf("incorrect input, please enter an integer between 1 and 100\n");
        while (getchar() != '\n')
            ;
    }
    size_data = size_data * 1000000;

    //getting the buffer size if the user chooses the circle buffer
    if (selected_prog == 4)
    {
        printf("Enter the desired size of buffer in Kilobytes (max 4KB) : \n");
        while (scanf("%d", &size_buffer) != 1 || size_buffer <= 0 || size_buffer > 4)
        {
            printf("incorrect input, please enter an integer between 1 and 4\n");
            while (getchar() != '\n')
                ;
        }
        size_buffer = size_buffer * 1000;
    }

    //processes to launch in background
    char processes_background[NUMBER_OF_PROCESSES_BACKGROUND][MAX_NAME_SIZE] =
        {"../unnamed_pipe/unnamed_pipe",
         "../named_pipe/named_pipe",
         "../socket/server",
         "../circle_buffer/writer",
         "../socket/client",};
    
    //get the sizes into char format
    char actualpath[100];
    int pids[NB_FORK];
    char size_data_char[MAX_NUM_SIZE];
    char size_buffer_char[MAX_NUM_SIZE];
    sprintf(size_data_char, "%i", size_data);
    sprintf(size_buffer_char, "%i", size_buffer); 

    //initialize children
    pid_t child = fork();
    if (child == -1)
    {
        //Error handling
        perror("fork");
        return EXIT_FAILURE;
    }
    if (child == 0)
    {
        //initialize processes
        if(selected_prog != 4){
            char *realpath_ = realpath(processes_background[selected_prog-1], actualpath);
            pids[0] = getpid();
            execlp(realpath_, realpath_, size_data_char, logname, NULL);
        }
        //add the buffer size if the user chose the circle buffer
        else{
            char *realpath_ = realpath(processes_background[selected_prog-1], actualpath);
            pids[0] = getpid();
            execlp(realpath_, realpath_, size_data_char,size_buffer_char, logname, NULL);
        }
    }
    //starting the client if the user chose the socket
    if (selected_prog == 3)
    {
        sleep(1);
        pid_t child_2 = fork();
        if (child_2 == -1)
        {
            //Error handling
            perror("fork");
            return EXIT_FAILURE;
        }
        if (child_2 == 0)
        {

            pids[1] = getpid();
            char *realpath_2 = realpath(processes_background[4], actualpath);
            execlp(realpath_2, realpath_2, size_data_char, logname, NULL);
        }
    }
    //wait for children termination
    waitForAll();
    exit(EXIT_SUCCESS);
}