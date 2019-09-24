#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *sorting(void *param); // function for sorting half array

typedef struct
{
	int *start; // the start address of the array
	int cnt;
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
	if (param->cnt == 0 || param->cnt == 1) pthread_exit(0);
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
	int lcnt = 0;
	int rcnt = 0;
	int *larray = (int*)malloc(param->cnt * sizeof(int));
	int *rarray = (int*)malloc(param->cnt * sizeof(int));
	int pivot = *(param->start);
	for (int i = 1; i < param->cnt; ++i)
	{
		if (*(param->start + i) <= pivot)
		{
			*(larray + lcnt) = *(param->start + i);
			lcnt++;
		}
		else
		{
			*(rarray + rcnt) = *(param->start + i);
			rcnt++;
		}
	}

	pthread_t tid1, tid2;
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        parameters *para1 = (parameters*)malloc(sizeof(parameters));
	parameters *para2 = (parameters*)malloc(sizeof(parameters));

	para1->start = larray;
	para1->cnt = lcnt;
	para2->start = rarray;
	para2->cnt = rcnt;

        pthread_create(&tid1, &attr, sorting, para1);
        // tid1 for sorting the left half

        pthread_create(&tid2, &attr, sorting, para2);
        // tid2 for sorting the right half

        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);
	
	// merge stage
	int i = 0;
	for (; i < lcnt; ++i)
		*(param->start + i) = *(larray + i);
	*(param->start + i) = pivot;
	i++;
	int j = 0;
	for (; j < rcnt; ++j)
		*(param->start + i + j) = *(rarray + j);
	
	// free stage
	free(para1);
	free(para2);
	free(larray);
	free(rarray);
	pthread_exit(0);
}
