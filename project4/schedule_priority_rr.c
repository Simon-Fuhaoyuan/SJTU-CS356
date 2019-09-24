#include "list.h"
#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

struct node *head = NULL;

void add(char *name, int priority, int burst)
{
	Task *newTask = malloc(sizeof(Task));
	newTask->name = name;
	newTask->priority = priority;
	newTask->burst = burst;
	newTask->flag = 0;
	newTask->startTime = 0;
	newTask->lastEndTime = 0;

	if (head == NULL)
	{
		head = malloc(sizeof(struct node));
		head->task = newTask;
		head->next = NULL;
	}
	else insert(&head, newTask);
}

Time* schedule()
{
	Time *timeptr = (Time*)malloc(sizeof(Time));
	timeptr->AveResponseTime = 0;
	timeptr->AveTurnAroundTime = 0;
	timeptr->AveWaitingTime = 0;
	int sysTime = 0;
	//traverse(head);
	struct node *temp = head;
	struct node *serve = head;
	int priority = 0;
	int serveTime = 0;

	while (head != NULL)
	{
		temp = head;
		priority = 0;
		while (temp != NULL)
		{
			if (temp->task->priority >= priority)
			{
				serve = temp;
				priority = temp->task->priority;
			}
			temp = temp->next;
		}
		if (serve->task->flag == 0)
		{
			serve->task->flag = 1;
			serve->task->startTime = sysTime;
			timeptr->AveWaitingTime += sysTime;
			timeptr->AveResponseTime += sysTime;
		}
		else
		{
			timeptr->AveWaitingTime += sysTime - serve->task->lastEndTime;
		}
		serveTime = QUANTUM > serve->task->burst ? serve->task->burst : QUANTUM;
		run(serve->task, serveTime);
		sysTime += serveTime;
		serve->task->lastEndTime = sysTime;
		serve->task->burst -= serveTime;
		delete(&head, serve->task);
		if (serve->task->burst != 0) insert(&head, serve->task);
		else timeptr->AveTurnAroundTime += serve->task->lastEndTime;
	}
	return timeptr;
}