/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

typedef struct
{
    pthread_t tid;
    int working;
    int index;
}
thread;

// the work
task worktodo;

// the thread pool
thread pool[NUMBER_OF_THREADS];

// the task queue
task queue[QUEUE_SIZE];

// the number of tasks in queue
int queue_cnt;

// the semaphore for threads
sem_t sem;

// the mutex locks
pthread_mutex_t mutex;
pthread_mutex_t mutex1;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    if (queue_cnt == 10) return 1;
    pthread_mutex_lock(&mutex1);
    queue[queue_cnt] = t;
    queue_cnt++;
    pthread_mutex_unlock(&mutex1);
    return 0;
}

// remove a task from the queue
task dequeue() 
{
    if (queue_cnt > 0)
    {
	pthread_mutex_lock(&mutex1);
        worktodo = queue[0];
        queue_cnt--;
        for (int i = 0; i < queue_cnt; ++i)
	    queue[i] = queue[i + 1];
	queue[queue_cnt].function = NULL;
	queue[queue_cnt].data = NULL;
	pthread_mutex_unlock(&mutex1);
    }
    else 
    {
	printf("No tasks to wait!\n");
	worktodo.function = NULL;
	worktodo.data = NULL;
    }
    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    // execute the task
    thread *threadp = (thread*)param;
    //printf("QueueCnt is %d\n", queue_cnt);
    worktodo = dequeue();
    execute(worktodo.function, worktodo.data);
    pool[threadp->index].working = 0;
    //printf("after changing %d is %d\n", threadp->index, pool[threadp->index].working);
    sem_post(&sem);
    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    worktodo.function = somefunction;
    worktodo.data = p;
    int value = enqueue(worktodo);
    //printf("pool_submit\n");
    if(value == 0)
    {
	sem_wait(&sem);
	pthread_mutex_lock(&mutex);
	int Id = 0;
	while(1)
	{
	    if (pool[Id].working == 0)
	    {
		pool[Id].working = 1;
		//printf("%d OK\n", Id);
		break;
	    }
	    else
	    {
		//printf("%d not OK\n", Id);
		Id++;
		if (Id == NUMBER_OF_THREADS) Id = 0;
	    }
	}
	//printf("In thread %d\n", Id);
	pthread_mutex_unlock(&mutex);
	pthread_create(&(pool[Id].tid),NULL,worker,&(pool[Id]));
    }
    return value;
}

// initialize the thread pool
void pool_init(void)
{
    //printf("pool_init\n");
    queue_cnt = 0;
    sem_init(&sem, 0, NUMBER_OF_THREADS);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex1, NULL);
    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
	pool[i].working = 0;
	pool[i].index = i;
    }
}

// shutdown the thread pool
void pool_shutdown(void)
{
    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
	pthread_join(pool[i].tid,NULL);
    sem_destroy(&sem);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex1);
}
