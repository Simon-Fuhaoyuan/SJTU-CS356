/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;
	int TaskNum = 0;

    in = fopen(argv[1],"r");
    
    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        // add the task to the scheduler's list of tasks
        add(name,priority,burst);
		TaskNum++;
        free(temp);
    }

    fclose(in);
	Time *timeptr = (Time*)malloc(sizeof(Time));
    // invoke the scheduler
    timeptr = schedule();

	printf("Average turnaround time is %f\n", timeptr->AveTurnAroundTime / TaskNum);
	printf("Average waiting time is %f\n", timeptr->AveWaitingTime / TaskNum);
	printf("Average response time is %f\n", timeptr->AveResponseTime / TaskNum);

    return 0;
}
