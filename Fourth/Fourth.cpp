#include <iostream>
#include <time.h>
#include "mpi.h"

#define ElementsPerProc 10

int* get_random_arr(int length);
void custom_bcast(int* a, int* b, int world_size, int elements_count);
void vector_sum(int* a, int* b, int length);
void vector_mult(int* a, int* b, int length);
void receive_and_gather(int* c, int world_size, int elements_count);

int main()
{
	srand(time(NULL));
	int rank;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0)
	{
		int world_size;
		MPI_Comm_size(MPI_COMM_WORLD, &world_size);
		int ArraySize = ElementsPerProc * (world_size - 1);
		int* a = get_random_arr(ArraySize);
		printf("Array A generated: ");
		for (int i = 0; i < ArraySize; i++)
			printf("%u ", a[i]);
		printf("\n\n");
		int* b = get_random_arr(ArraySize);
		printf("Array B generated: ");
		for (int i = 0; i < ArraySize; i++)
			printf("%u ", b[i]);
		printf("\n\n");
		int* c = get_random_arr(ArraySize);

		custom_bcast(a, b, world_size, ElementsPerProc);
		receive_and_gather(c, world_size, ElementsPerProc);

		printf("Result C array: ");
		for (int i = 0; i < ArraySize; i++)
			printf("%u ", c[i]);

		free(c);
		free(b);
		free(a);
	}
	else
	{
		int length;
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &length);
		int* a = (int*)malloc(sizeof(int) * length);
		int* b = (int*)malloc(sizeof(int) * length);
		MPI_Recv(a, length, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(b, length, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		vector_sum(a, b, length);
		MPI_Send(a, length, MPI_INT, 0, 0, MPI_COMM_WORLD);
		free(a);
		free(b);
	}
	MPI_Finalize();
}

int* get_random_arr(int length)
{
	int* arr;
	arr = (int*)malloc(sizeof(int) * length);
	for (int i = 0; i < length; i++)
		arr[i] = rand();
	return arr;
}

void custom_bcast(int* a, int* b, int world_size, int elements_count)
{
	for (int i = 1; i < world_size; i++)
	{
		MPI_Send(a + (i - 1) * elements_count, elements_count, MPI_INT, i, 0, MPI_COMM_WORLD);
		MPI_Send(b + (i - 1) * elements_count, elements_count, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
}

void vector_sum(int* a, int* b, int length)
{
	for (int i = 0; i < length; i++)
	{
		a[i] += b[i];
	}
}

void vector_mult(int* a, int* b, int length)
{
	for (int i = 0; i < length; i++)
	{
		a[i] *= b[i];
	}
}

void receive_and_gather(int* c, int world_size, int elements_count)
{
	for (int i = 1; i < world_size; i++)
	{
		MPI_Recv(c + (i - 1) * elements_count, elements_count, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
}