#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *sorting(void *param); // function for sorting half array
void *merging(void *param); // function for merging

typedef struct
{
	int *start; // the start address of the array
	int cnt; // the count of numbers waiting for sorting
}parameters;

int main(int argc, char *argv[])
{
	int *array; // store the input array
	int size;
	
	array = (int *)malloc((argc - 1) * sizeof(int));
	size = argc - 1;
	for (int i = 0; i < size; ++i)
		*(array + i) = atoi(argv[i + 1]);
	
	parameters *para = (parameters*)malloc(sizeof(parameters));
	para->start = array;
	para->cnt = size;

	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&tid, &attr, sorting, para);
	pthread_join(tid, NULL);

	for (int i = 0; i < size; ++i)
		printf("%d ", *(array + i));
	printf("\n");

	free(array);
	free(para);

	return 0;
}

void *sorting(void *para)
{
	parameters *param = (parameters*) para;
	
	// special cases
	if (param->cnt == 1) pthread_exit(0);
	if (param->cnt == 2)
	{
		if (*(param->start) > *(param->start + 1))
		{
			int temp = *(param->start);
			*(param->start) = *(param->start + 1);
			*(param->start + 1) = temp;
		}
		pthread_exit(0);
	}
	
	// normal cases
	pthread_t tid1, tid2, tid3;
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        parameters *para1 = (parameters*)malloc(sizeof(parameters));
	parameters *para2 = (parameters*)malloc(sizeof(parameters));

	para1->start = param->start;
	para1->cnt = param->cnt / 2;
	para2->start = param->start + para1->cnt;
	para2->cnt = param->cnt - para1->cnt;

        pthread_create(&tid1, &attr, sorting, para1);
        // tid1 for sorting the first half

        pthread_create(&tid2, &attr, sorting, para2);
        // tid2 for sorting the second half

        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);
	
	pthread_create(&tid3, &attr, merging, param);
        // tid3 for merging
        pthread_join(tid3, NULL);

	free(para1);
	free(para2);
	pthread_exit(0);
}

void *merging(void *para)
{
	parameters *param = (parameters*)para;
	int *assist = (int *)malloc(param->cnt * sizeof(int));
	int index1 = 0, index2 = param->cnt / 2;
	int time = 0;

	while(index1 < param->cnt / 2 && index2 < param->cnt)
	{
		if (*(param->start + index1) < *(param->start + index2))
		{
			*(assist + time) = *(param->start + index1);
			index1++;
		}
		else
		{
			*(assist + time) = *(param->start + index2);
			index2++;
		}
		time++;
	}

	if (index1 == param->cnt / 2)
		for (; index2 < param->cnt; index2++)
		{
			*(assist + time) = *(param->start + index2);
			time++;
		}
	else
		for (; index1 < param->cnt / 2; index1++)
		{
			*(assist + time) = *(param->start + index1);
			time++;
		}

	for (int i = 0; i < param->cnt; ++i)
		*(param->start + i) = *(assist + i);

	free(assist);

	pthread_exit(0);
}
