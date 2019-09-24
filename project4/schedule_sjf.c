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
	int shortest = 9999;

	while (head != NULL)
	{
		temp = head;
		shortest = 9999;
		while (temp != NULL)
		{
			if (temp->task->burst <= shortest)
			{
				serve = temp;
				shortest = temp->task->burst;
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
		run(serve->task, serve->task->burst);
		sysTime += serve->task->burst;
		serve->task->lastEndTime = sysTime;
		delete(&head, serve->task);
		timeptr->AveTurnAroundTime += serve->task->lastEndTime;
	}
	return timeptr;
}
