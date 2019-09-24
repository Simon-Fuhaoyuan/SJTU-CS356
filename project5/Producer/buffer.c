#include "buffer.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

/* the empty semaphore */
sem_t empty;

/* the full semaphore */
sem_t full;

/* the mutex */
pthread_mutex_t mutex;

void buffer_init()
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    buffer[0] = '\0';
}

int insert_item(buffer_item item)
{
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    int i = 0;
    while(buffer[i] != '\0') i++;
    buffer[i] = item;
    buffer[i + 1] = '\0';
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    
    return 0;
}

int remove_item(buffer_item *item)
{
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    *item = buffer[0];
    int i = 0;
    while(buffer[i] != '\0')
    {
	buffer[i] = buffer[i + 1];
	i++;
    }
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);

    return 0;
}
