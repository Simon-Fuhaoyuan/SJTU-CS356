/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;
    sleep(3);
    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
    /*if (temp->a == 0 || temp->b == 0) printf("finish\n");
    else
    {
	temp->b -= temp->a;
	add(temp);
    }*/
}

int main(void)
{
    // create some work to do
    struct data work;
    work.a = 1;
    work.b = 1;

    struct data work1;
    work1.a = 2;
    work1.b = 2;

    struct data work2;
    work2.a = 3;
    work2.b = 3;

    struct data work3;
    work3.a = 4;
    work3.b = 4;

    struct data work4;
    work4.a = 5;
    work4.b = 5;

    struct data work5;
    work5.a = 6;
    work5.b = 6;
    
    struct data work6;
    work6.a = 7;
    work6.b = 7;
    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    pool_submit(&add,&work);
    //sleep(3);
    pool_submit(&add, &work1);
    //sleep(3);
    pool_submit(&add, &work2);
    pool_submit(&add, &work3);
    pool_submit(&add, &work4);
    pool_submit(&add, &work5);
    pool_submit(&add, &work6);
    // may be helpful 
    //sleep(3);

    pool_shutdown();

    return 0;
}
