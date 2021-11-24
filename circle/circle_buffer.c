#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define BSIZE 10
#define DATASZ 1000

typedef struct shared_buffer
{
    pthread_mutex_t lock;
    pthread_cond_t
        new_data_cond,
        new_space_cond;
    char c[BSIZE];
    int next_in,
        next_out,
        count;
} shared_buffer_t;

void sb_init(shared_buffer_t *sb)
{
    sb->next_in = sb->next_out = sb->count = 0;
    pthread_mutex_init(&sb->lock, NULL);
    pthread_cond_init(&sb->new_data_cond, NULL);
    pthread_cond_init(&sb->new_space_cond, NULL);
}

void *producer(void *arg)
{
    char message[DATASZ] = "";
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
    int k = 0;
    i = 0;
    shared_buffer_t *sb = (shared_buffer_t *)arg;

    while(1)
    {
        pthread_mutex_lock(&sb->lock);
        while (sb->count == BSIZE)
            pthread_cond_wait(&sb->new_space_cond, &sb->lock);
        pthread_mutex_unlock(&sb->lock);
        k = sb->next_in;
        if (i > DATASZ -1){
            pthread_exit(NULL);
        }
        sb->c[k] = message[i];
        i++;
        sb->next_in = (sb->next_in + 1) % BSIZE;
        pthread_mutex_lock(&sb->lock);
        sb->count++;
        pthread_mutex_unlock(&sb->lock);
        pthread_cond_signal(&sb->new_data_cond);
    }
}

void *consumer(void *arg)
{
    int count = 0;
    int k = 0;
    shared_buffer_t *sb = (shared_buffer_t *)arg;
    while(1){
        pthread_mutex_lock(&sb->lock);
    while (sb->count == 0)
        pthread_cond_wait(&sb->new_data_cond, &sb->lock);
    pthread_mutex_unlock(&sb->lock);
    k = sb->next_out;
    char new_char = sb->c[k];
    printf("%c", new_char);
    fflush(stdout);
    count ++;
    if (count > DATASZ - 1){
        pthread_exit(NULL);
    }
    sb->next_out = (sb->next_out + 1) % BSIZE;
    pthread_mutex_lock(&sb->lock);
    sb->count--;
    pthread_mutex_unlock(&sb->lock);
    pthread_cond_signal(&sb->new_space_cond);
    }
}

int main()
{
    pthread_t th1, th2;
    shared_buffer_t sb;

    sb_init(&sb);
    pthread_create(&th1, NULL, producer, &sb);
    pthread_create(&th2, NULL, consumer, &sb);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    return 0;
}