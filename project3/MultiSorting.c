#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int *array; // store the input array
int size;
void *sorting (void *start); // function for sorting half array
void *merging (void *param); // function for merging the array

int main(int argc, char *argv[])
{
	array = (int *)malloc((argc - 1) * sizeof(int));
	size = argc - 1;
	for (int i = 0; i < size; ++i)
		*(array + i) = atoi(argv[i + 1]);

	pthread_t tid1, tid2, tid3; // the thread identifier
				    // tid1 and tid2 used for sorting half array.
				    // tid3 used for merging
	pthread_attr_t attr; // set of thread attributes

	pthread_attr_init(&attr);
       	// set the default attributes of the thread
	
	int Addr1 = 0;
	int Addr2 = size / 2;
	// start address for different half

	pthread_create(&tid1, &attr, sorting, &Addr1);
	// tid1 for sorting the first half

	pthread_create(&tid2, &attr, sorting, &Addr2);
	// tid2 for sorting the second half

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	// get the result of half-sorted array
	/*
	for (int i = 0; i < size; ++i)
                printf("%d ", *(array + i));
        printf("\n");
	*/
	pthread_create(&tid3, &attr, merging, &size);
	// tid3 for merging
	pthread_join(tid3, NULL);

	for (int i = 0; i < size; ++i)
		printf("%d ", *(array + i));
	printf("\n");

	free(array);
}

void *sorting(void *start)
{
	int *startAddr = start;
	int endAddr = *startAddr == 0 ? size / 2 : size; // get the end address for different half

	for (int i = *startAddr; i < endAddr; ++i)
		for (int j = i + 1; j < endAddr; ++j)
			if (*(array + i) > *(array + j))
			{
				int temp = *(array + i);
				*(array + i) = *(array + j);
				*(array + j) = temp;
			}

	pthread_exit(0);
}

void *merging(void *param)
{
	int *assist = (int *)malloc(size * sizeof(int));
	int index1 = 0, index2 = size / 2;
	int time = 0;

	while(index1 < size / 2 && index2 < size)
	{
		if (*(array + index1) < *(array + index2))
		{
			*(assist + time) = *(array + index1);
			index1++;
		}
		else
		{
			*(assist + time) = *(array + index2);
			index2++;
		}
		time++;
	}

	if (index1 == size / 2)
		for (; index2 < size; index2++)
		{
			*(assist + time) = *(array + index2);
			time++;
		}
	else
		for (; index1 < size / 2; index1++)
		{
			*(assist + time) = *(array + index1);
			time++;
		}

	for (int i = 0; i < size; ++i)
		*(array + i) = *(assist + i);

	free(assist);
	pthread_exit(0);
}
