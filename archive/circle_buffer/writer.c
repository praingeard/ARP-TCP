#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

char shm_fn[] = "my_shm";
char sem_fn[] = "my_sem_1";
char sem2_fn[] = "my_sem_2";
/**** WRITER ****/
int main()
{
    size_t DATASZ = 100;
    size_t buffersize = 5;
    
    caddr_t shmptr;
    unsigned int mode;
    int shmdes, index;
    sem_t *sem_new_space;
    sem_t *sem_new_data;
    int SHM_SIZE;
    mode = S_IRWXU | S_IRWXG;
    /* Open the shared memory object */
    if ((shmdes = shm_open(shm_fn, O_CREAT | O_RDWR | O_TRUNC, mode)) == -1)
    {
        perror("shm_open failure");
        return 1;
    }
    /* Preallocate a shared memory area */
    SHM_SIZE = sysconf(_SC_PAGE_SIZE);
    if (ftruncate(shmdes, SHM_SIZE) == -1)
    {
        perror("ftruncate failure");
        return 1;
    }
    if ((shmptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED,
                       shmdes, 0)) == (caddr_t)-1)
    {
        perror("mmap failure");
        return 1;
    }
    /* Create a semaphore in locked state */
    sem_new_data = sem_open(sem_fn, O_CREAT, 0644, 0);
    if (sem_new_data == (void *)-1)
    {
        perror("sem_open failure");
        return 1;
    }
    sem_init(sem_new_data, 1,0);
    sem_new_space = sem_open(sem2_fn, O_CREAT, 0644, 0);
    if (sem_new_space == (void *)-1)
    {
        perror("sem_open failure");
        return 1;
    }
    sem_init(sem_new_space, 1,0);
   
    // printf("%li", DATASZ);
    // fflush(stdout);
    char message[DATASZ];
    int i, n, rnd;
    srand(time(NULL));
    for (i = 0; i < DATASZ - 1; ++i)
    {
        rnd = rand();
        n = (rnd >> 4) & 0xF;
        *(message + i) = (rnd & 0xF) & 1
                             ? (n % 10) + '0'
                             : (n % 26) + 'A';
    }
    message[DATASZ - 1] = 0;
    int next_in = 0;
    int count = 0;
    i = 0;

    while(1)
    {
       
        if (count == buffersize){
            printf("waiting new space\n");
             sem_wait(sem_new_space);
             count --;
        }
        if (i > DATASZ -1){
            break;
        }
        shmptr[next_in] = message[i];
        i++;
        next_in = (next_in + 1) % buffersize;
        count++;
        printf("write data number %i\n", i);
        sem_post(sem_new_data);
    }
    /* Release the semaphore lock */
    munmap(shmptr, SHM_SIZE);
    /* Close the shared memory object */
    close(shmdes);
    /* Close the Semaphore */
    sem_close(sem_new_data);
    sem_close(sem_new_space);
    /* Delete the shared memory object */
    shm_unlink(shm_fn);
    return 0;
}