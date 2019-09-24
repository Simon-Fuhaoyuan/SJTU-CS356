#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define holeMax 20
#define MaxInt 2147483647

typedef struct
{
    bool flag; // detect if this hole is valid
    int start;
    int end;
    char name[10];
    bool use; // detect if this hole is used or not
}hole;

// Global variables
hole memory[holeMax];
int holeNum = 0;

// auxiliary functions
bool request(int size, char *name, char *mode);
void release(char *name);
void compact();
void showState();

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
	printf("Wrong arguments!\n");
	return 0;
    }

    int size = atoi(argv[1]);

    memory[0].flag = true;
    memory[0].start = 0;
    memory[0].end = size - 1;
    memory[0].use = false;
    holeNum++;

    for (int i = 1; i < holeMax; ++i)
    {
	memory[i].flag = false;
	memory[i].use = false;
    }

    char buffer[5];
    while(1)
    {
	printf("allocator> ");
	scanf("%s", buffer);
	if(!strcmp(buffer, "X"))
	    break;
	else if (!(strcmp(buffer, "RQ")))
	{
	    int alloc;
	    char aname[10];
	    char mode[2];
	    scanf("%s%d%s", aname, &alloc, mode);
	    request(alloc, aname, mode);
	}
	else if (!(strcmp(buffer, "RL")))
	{
	    char aname[10];
	    scanf("%s", aname);
	    release(aname);
	}
	else if (!(strcmp(buffer, "C")))
        {
	    compact();
        }
	else if (!(strcmp(buffer, "STAT")))
        {
	    showState();
        }
	else
	    printf("Wrong input!\n");
    }
    return 0;
}

bool request(int size, char *name, char *mode)
{
    int index = 0;
    bool found = false;
    if (!strcmp(mode, "F"))
    {
	while (memory[index].flag)
	{
	    if (!memory[index].use && (memory[index].end - memory[index].start + 1) >= size)
	    {
		found = true;
		break;
	    }
	    index++;
	}
    }
    else if (!strcmp(mode, "B"))
    {
	int minimum = MaxInt;
	int tempIndex = 0;
	while (memory[tempIndex].flag)
	{
	    if (!memory[tempIndex].use && (memory[tempIndex].end - memory[tempIndex].start + 1) >= size && (memory[tempIndex].end - memory[tempIndex].start + 1) < minimum)
	    {
		found = true;
		index = tempIndex;
		minimum = memory[tempIndex].end - memory[tempIndex].start + 1;
	    }
	    tempIndex++;
	}
    }
    else if (!strcmp(mode, "W"))
    {
	int maximum = 0;
	int tempIndex = 0;
	while (memory[tempIndex].flag)
	{
	    if (!memory[tempIndex].use && (memory[tempIndex].end - memory[tempIndex].start + 1) >= size && (memory[tempIndex].end - memory[tempIndex].start + 1) > maximum)
	    {
		found = true;
		index = tempIndex;
		maximum = memory[tempIndex].end - memory[tempIndex].start + 1;
	    }
	    tempIndex++;
	}
    }
    else
    {
	printf("Bad flag!\n");
	return false;
    }

    // cannot find a proper address
    if (!found)
    {
	printf("Cannot find a proper address to allocate this process!\n");
	return false;
    }
    
    // find a proper address
    // size of the hole is exactly that of the process
    if ((memory[index].end - memory[index].start + 1) == size)
    {
	strcpy(memory[index].name, name);
	memory[index].use = true;
	return true;
    }
    // normal instances
    for (int i = holeNum - 1; i >= index; --i)
    {
	memory[i + 1].flag = true;
	memory[i + 1].start = memory[i].start;
	memory[i + 1].end = memory[i].end;
	strcpy(memory[i + 1].name, memory[i].name);
	memory[i + 1].use = memory[i].use;
    }
    memory[index].end = size + memory[index].start - 1;
    memory[index].use = true;
    strcpy(memory[index].name, name);
    memory[index + 1].start = memory[index].end + 1;
    holeNum++;
    return true;
}

void release(char *name)
{
    int index = 0;
    for (; index < holeNum; ++index)
    {
	if (!strcmp(name, memory[index].name) && memory[index].use)
	    break;
    }

    // cannot find the relating process
    if (index == holeNum)
    {
	printf("NO such process!\n");
	return;
    }

    memory[index].use = false;
    // check the situation of adjacent hole
    // if adjacent hole is not in use, compact them
    if (index + 1 < holeNum && memory[index + 1].flag && !memory[index + 1].use)
    {
	memory[index].end = memory[index + 1].end;
	for (int i = index + 1; i < holeNum; ++i)
	{
	    memory[i].flag = memory[i + 1].flag;
	    memory[i].start = memory[i + 1].start;
	    memory[i].end = memory[i + 1].end;
	    strcpy(memory[i].name, memory[i + 1].name);
	    memory[i].use = memory[i + 1].use;
	}
	holeNum--;
    }
    if (index - 1 >= 0 && !memory[index - 1].use)
    {
	memory[index - 1].end = memory[index].end;
	for (int i = index; i < holeNum; ++i)
        {
            memory[i].flag = memory[i + 1].flag;
            memory[i].start = memory[i + 1].start;
            memory[i].end = memory[i + 1].end;
            strcpy(memory[i].name, memory[i + 1].name);
            memory[i].use = memory[i + 1].use;
        }
        holeNum--;
    }
}

void compact()
{
    int assist[holeMax];
    int processNum = 0;
    int unused = 0;
    for (int i = 0; i < holeNum; ++i)
    {
	if (memory[i].use)
	{
	    assist[processNum] = i;
	    processNum++;
	}
	else
	    unused += memory[i].end - memory[i].start + 1;
    }

    // no process in memory
    if (processNum == 0) return;

    // compact stage
    for (int i = 0; i < processNum; ++i)
    {
	memory[i].flag = true;
	memory[i].start = i > 0 ? memory[i - 1].end + 1 : 0;
	memory[i].end = memory[assist[i]].end - memory[assist[i]].start + memory[i].start;
	strcpy(memory[i].name, memory[assist[i]].name);
	memory[i].use = true;
    }
    memory[processNum].flag = true;
    memory[processNum].start = memory[processNum - 1].end + 1;
    memory[processNum].end = memory[processNum].start + unused - 1;
    memory[processNum].use = false;
    for(int i = processNum + 1; i < holeMax; ++i)
    {
	memory[i].flag = false;
	memory[i].use = false;
    }
    holeNum = processNum + 1;
}

void showState()
{
    for (int i = 0; i < holeNum; i++)
    {
	if (memory[i].use)
	    printf("Addresses [%d:%d] Process %s\n", memory[i].start, memory[i].end, memory[i].name);
	else
	    printf("Addresses [%d:%d] Unused\n", memory[i].start, memory[i].end);
    }
}
