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

pid_t getprogrampid(char *name)
{
    char line[80];
    char command[] = "pidof ";
    strcat(command, name);
    FILE *cmd = popen(command, "r");
    fgets(line, 80, cmd);
    pid_t pid = strtoul(line, NULL, 10);
    printf("pid_process %i\n", pid);
    fflush(stdout);
    pclose(cmd);
    return pid;
}

int main()
{
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

    printf("Enter the desired programs : \n ");
    printf("1 : unnamed pipe \n ");
    printf("2 : named pipe \n ");
    printf("3 : socket \n ");
    printf("4 : circle buffer \n ");
    scanf("%i", &selected_prog);

    printf("Enter the desired size of data to send in bytes : \n ");
    scanf("%i", &size_data);

    if (selected_prog == 4)
    {
        printf("Enter the desired size of buffer in bytes : \n");
        scanf("%i", &size_buffer);
    }

    //processes to launch in background
    char processes_background[NUMBER_OF_PROCESSES_BACKGROUND][MAX_NAME_SIZE] =
        {"../unnamed_pipe/unnamed_pipe",
         "../named_pipe/named_pipe",
         "../socket/server",
         "../circle_buffer/circle_buffer",
         "../socket/client"};
    char actualpath[100];
    int pids[NB_FORK];
    char size_data_char[MAX_NUM_SIZE];
    char size_buffer_char[MAX_NUM_SIZE];
    sprintf(size_data_char, "%i", size_data);
    sprintf(size_buffer_char, "%i", size_data);
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
        char* realpath_ = realpath(processes_background[selected_prog-1], actualpath);
        if (selected_prog == 4)
        {
            pids[0] = getpid();
            execlp(realpath_,realpath_, size_data_char, size_buffer_char, logname, NULL);
        }
        else
        {
            pids[0] = getpid();
            execlp(realpath_,realpath_, size_data_char, logname, NULL);
        }
    }
    if (selected_prog == 3)
    {
        child = fork();
        if (child == -1)
        {
            //Error handling
            perror("fork");
            return EXIT_FAILURE;
        }
        if (child == 0)
        {
            //exec in the background
            char * realpath_ = realpath(processes_background[4], actualpath);
            pids[1] = getpid();
            execlp(realpath_,realpath_, size_data_char, logname, NULL);
        }
    }

    //show pids
    printf("Processes started with pids :");
    for (int i = 0; i < NB_FORK; i++)
    {
        printf(" %i ", pids[i]);
    }
    printf("\n");
    fflush(stdout);
    waitForAll();
    exit(EXIT_SUCCESS);
}