#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

char shm_fn[] = "my_shm";
char sem_fn[] = "my_sem_1";
char sem2_fn[] = "my_sem_2";
/**** READER ****/
int main()
{
    size_t DATASZ = 100;
    size_t buffersize = 5;

    caddr_t shmptr;
    int shmdes, index;
    sem_t *sem_new_space;
    sem_t *sem_new_data;
    int SHM_SIZE;
    /* Open the shared memory object */
    SHM_SIZE = sysconf(_SC_PAGE_SIZE);
    if ((shmdes = shm_open(shm_fn, O_RDWR, 0)) == -1)
    {
        perror("shm_open failure");
        return 1;
    }
    if ((shmptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED,
                       shmdes, 0)) == (caddr_t)-1)
    {
        perror("mmap failure");
        return 1;
    }
    /* Open the Semaphore */
    sem_new_data = sem_open(sem_fn, 0, 0644, 0);
    if (sem_new_data == (void *)-1)
    {
        perror("sem_open failure");
        return 1;
    }
    sem_new_space = sem_open(sem2_fn, 0, 0644, 0);
    if (sem_new_space == (void *)-1)
    {
        perror("sem_open failure");
        return 1;
    }
    /* Lock the semaphore */
    printf("waiting \n");

    int count = 0;
    int next_out = 0;
    while (1)
    {
        sem_wait(sem_new_data);
        char new_char = shmptr[next_out];
        printf("%c", new_char);
        fflush(stdout);
        count++;
        if (count > DATASZ - 1)
        {
            break;
        }
        next_out = (next_out + 1) % buffersize;
        sem_post(sem_new_space);
    }
    munmap(shmptr, SHM_SIZE);
    /* Close the shared memory object */
    close(shmdes);
    /* Close the Semaphore */
    sem_close(sem_new_space);
    sem_close(sem_new_data);
    sem_unlink(sem_fn);
}