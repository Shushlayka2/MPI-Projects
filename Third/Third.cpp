#include <iostream>
#include <time.h>
#include "mpi.h"

#define MaxArraySize 10

int* get_random_arr(int* length);
void custom_bcast(void* data, int count);

int main()
{
	int rank;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 1)
	{
		int length;
		int* a = get_random_arr(&length);
		custom_bcast(a, length);
		printf("Process 1 sent array of nums: ");
		for (int i = 0; i < length; i++)
			printf("%u ", a[i]);
		printf("\n");
		free(a);
	}
	else
	{
		int a_length;
		MPI_Status status;
		MPI_Probe(1, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &a_length);
		int* a = (int*)malloc(sizeof(int) * a_length);
		MPI_Recv(a, a_length, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process %u received %u length array of nums: ", rank, a_length);
		for (int i = 0; i < a_length; i++)
			printf("%u ", a[i]);
		printf("\n");
		free(a);
	}
	MPI_Finalize();
}

int* get_random_arr(int* length)
{
	int* arr;
	srand(time(0));
	*length = rand() % MaxArraySize;
	arr = (int*)malloc(sizeof(int) * *length);
	for (int i = 0; i < *length; i++)
		arr[i] = rand();
	return arr;
}

void custom_bcast(void* data, int count) 
{
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	for (int i = 0; i < world_size; i++) 
	{
		if (i != 1) 
		{
			MPI_Send(data, count, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
}