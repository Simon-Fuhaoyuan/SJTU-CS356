#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
bool check_safe_state(int (*need)[NUMBER_OF_RESOURCES], int available[]);

int main(int argc, char *argv[])
{
    /* get the available array from command line */
    if (argc >= 5)
    {
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
	    available[i] = atoi(argv[i + 1]);
    }
    else
    {
	printf("Too few arguments!\n");
        return 0;	
    }
    
    /* read the maximum array from file */
    FILE *fp;
    if ((fp = fopen("maximum.txt", "r")) == NULL)
    {
	printf("File cannot open!\n");
	return 0;
    }
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
	{
	    fscanf(fp, "%d", &maximum[i][j]);
	    fgetc(fp);
	}

    /* initialize the allocation and need array */
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
	{
	    allocation[i][j] = 0;
	    need[i][j] = maximum[i][j];
	}
    
    /* finish initialization */
    printf("Initialize finish!\n");

    /* user input stage */
    char input[10];
    int custNum;
    int array[NUMBER_OF_RESOURCES];
    while(true)
    {
	printf("user> ");
	scanf("%s", &input);

	/* request resources */
	if (strcmp(input, "RQ") == 0)
	{
	    scanf("%d %d %d %d %d", &custNum, array, array + 1, array + 2, array + 3);
	    int value = request_resources(custNum, array);
	    printf("%d\n", value);
	}

	/* release resources */
	else if (strcmp(input, "RL") == 0)
	{
	    scanf("%d %d %d %d %d", &custNum, array, array + 1, array + 2, array + 3);
	    release_resources(custNum, array);
	}

	/* show information */
	else if (strcmp(input, "*") == 0)
	{
	    printf("available:\n");
	    printf("R1 R2 R3 R4\n");
	    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
		printf("%d  ", available[j]);
            printf("\n");

	    printf("\nmaximum:\n");
	    printf("R1 R2 R3 R4\n");
	    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
            {
                for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
                    printf("%d  ", maximum[i][j]);
                printf("\n");
            }

	    printf("\nallocation:\n");
	    printf("R1 R2 R3 R4\n");
	    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
            {
                for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
                    printf("%d  ", allocation[i][j]);
                printf("\n");
            }

	    printf("\nneed:\n");
	    printf("R1 R2 R3 R4\n");
	    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
            {
                for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
                    printf("%d  ", need[i][j]);
                printf("\n");
            }
	}

	/* exit */
	else if(strcmp(input, "exit") == 0)
	    break;

	/* wrong input */
	else
	    printf("Wrong input!\n");
    }

    return 0;
}

int request_resources(int customer_num, int request[])
{
    /* normal check */
    if(customer_num > NUMBER_OF_CUSTOMERS - 1)
    {
        printf("Customer number too big!\n");
        return -1;
    }

    bool flag = true;
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
	if (need[customer_num][i] < request[i])
	    flag = false;

    if(!flag)
    {
	printf("Request resources too big!\n");
	return -1;
    }

    /* duplicate the available and need array */
    int copy_available[NUMBER_OF_RESOURCES];
    int copy_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
	{
	    copy_available[j] = available[j];
	    copy_need[i][j] = need[i][j];
	}

    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
	copy_need[customer_num][i] -= request[i];
	copy_available[i] -= request[i];
    }

    /* check if there's a safe state */
    flag = check_safe_state(copy_need, copy_available);
    
    if(flag)
    {
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        {
            allocation[customer_num][i] += request[i];
            available[i] -= request[i];
            need[customer_num][i] -= request[i];
        }
	return 0;
    }
    else return -1;
}

bool check_safe_state(int (*need)[NUMBER_OF_RESOURCES], int available[])
{
    /* special condition */
    bool allZero = true;
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
	    if(need[i][j] != 0) allZero = false;
    if(allZero) return true;

    /* normal condition */
    bool flag1 = true;    // detect available is enough
    bool flag2 = false;	  // detect the state of all zero
    bool flag3 = false;   // get the state of a possible allocation
    int copy_available[NUMBER_OF_RESOURCES];
    int copy_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    {
		flag1 = true;
		flag2 = false;
		flag3 = false;
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
		{
			if(need[i][j] > available[j]) flag1 = false;
			if(need[i][j] != 0) flag2 = true;
		}
		if(flag1 && flag2)
		{
    	    for (int m = 0; m < NUMBER_OF_CUSTOMERS; ++m)
        	for (int n = 0; n < NUMBER_OF_RESOURCES; ++n)
        	{
            	    copy_available[n] = available[n];
            	    copy_need[m][n] = need[m][n];
        	}
			for (int m = 0; m < NUMBER_OF_RESOURCES; ++m)
			{
				copy_available[m] -= copy_need[i][m];
				copy_available[m] += maximum[i][m];
				copy_need[i][m] = 0;
			}
			flag3 = check_safe_state(copy_need, copy_available);
			if (flag3) return true;
		}
    }

    /* not a safe state */
    return false;
}

void release_resources(int customer_num, int release[])
{
    /* normal check */
    if(customer_num > NUMBER_OF_CUSTOMERS - 1)
    {
	printf("Customer number too big!\n");
	return;
    }

    bool flag = true;
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
	if (allocation[customer_num][i] < release[i])
	    flag = false;

    if(flag)
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
	{
	    allocation[customer_num][i] -= release[i];
	    available[i] += release[i];
	    need[customer_num][i] += release[i];
	}
    else
	printf("Release resources too big!\n");
}
