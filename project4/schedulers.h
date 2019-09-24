#define MIN_PRIORITY 1
#define MAX_PRIORITY 10
#include "task.h"

typedef struct Time
{
	float AveWaitingTime;
	float AveTurnAroundTime;
	float AveResponseTime;
}Time;

// add a task to the list 
void add(char *name, int priority, int burst);

// invoke the scheduler
Time* schedule();
