#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "buffer.h"

void *producer(void *param);
void *consumer(void *param);

typedef struct thread
{
    pthread_t tid;
    struct thread *next; 
}LinkThread;

int main(int argc, char *argv[])
{
    /* initialize the buffer */
    buffer_init();

    /* get the arguments */
    int sleeptime, producer_cnt, consumer_cnt;
    if(argc >= 4)
    {
	sleeptime = atoi(argv[1]);
	producer_cnt = atoi(argv[2]);
	consumer_cnt = atoi(argv[3]);
    }
    else
    {
	printf("Too few arguments!\n");
	return 0;
    }
    
    struct thread *ProHead;
    struct thread *ConHead;
    struct thread *temp;
    struct thread *ProLast;
    struct thread *ConLast;

    /* initialize producer threads */
    for (int i = 0; i < producer_cnt; ++i)
    {
	temp = (struct thread*)malloc(sizeof(struct thread));
	if(i == 0) 
	{
	    ProHead = temp;
	    ProLast = temp;
	}
	else
	{
	    ProLast->next = temp;
	    ProLast = temp;
	}
	pthread_create(&(temp->tid), NULL, producer, NULL);
    }

    /* initialize consumer threads */
    for (int j = 0; j < consumer_cnt; ++j)
    {
        temp = (struct thread*)malloc(sizeof(struct thread));
        if(j == 0)
        {
            ConHead = temp;
            ConLast = temp;
        }
        else
        {
            ConLast->next = temp;
            ConLast = temp;
        }
        pthread_create(&(temp->tid), NULL, consumer, NULL);
    }

    /* sleep for a while */
    sleep(sleeptime);
    
    /* Exit stage */
    for (int i = 0; i < producer_cnt; ++i)
    {
	temp = ProHead;
	while(temp != ProLast)
	    temp = temp->next;
        pthread_cancel(temp->tid);
    }

    for (int i = 0; i < consumer_cnt; ++i)
    {
	temp = ConHead;
        while(temp != ConLast)
            temp = temp->next;
        pthread_cancel(temp->tid);
    }

    return 0;
}

void *producer(void *param)
{
    buffer_item item;

    while(1)
    {
	/* sleep for a random period of time */
	int sleeptime = rand() % 5;
	sleep(sleeptime);
	/* generate a random number */
	item = rand();
	if (insert_item(item))
	    printf("report error condition\n");
	else
	    printf("producer produced %d\n", item);

	pthread_testcancel();
    }
}

void *consumer(void *param)
{
    buffer_item item;

    while(1)
    {
	/* sleep for a random period of time */
	int sleeptime = rand() % 5;
	sleep(sleeptime);
	if (remove_item(&item))
	    printf("report error condition\n");
	else
	    printf("consumer consumed %d\n", item);

	pthread_testcancel();
    }
}
