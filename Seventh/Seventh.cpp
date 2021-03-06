﻿#include <iostream>
#include <time.h>
#include "mpi.h"

#define ElementsPerProc 2
#define AbsMaxVal 10

void generate_random_arr(int* a, int n);

int main()
{
	srand(time(NULL));
	int rank, world_size;
	int* x = NULL;
	int global_sum = 0;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int ArraySize = ElementsPerProc * world_size;

	if (rank == 0)
	{
		x = (int*)malloc(sizeof(int) * ArraySize);
		generate_random_arr(x, ArraySize);
		printf("Generated array: \n");
		for (int i = 0; i < ArraySize; i++)
			printf("%d ", x[i]);
		printf("\n");
	}

	int* sub_x = (int*)malloc(sizeof(int) * ElementsPerProc);
	MPI_Scatter(x, ElementsPerProc, MPI_INT, sub_x, ElementsPerProc, MPI_INT, 0, MPI_COMM_WORLD);
	int local_sum = 0;
	for (int i = 0; i < ElementsPerProc; i++)
		local_sum += abs(sub_x[i]);
	printf("Local sum of rank %u equals to %d\n", rank, local_sum);
	MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf("Total sum = %d\n", global_sum);
	MPI_Finalize();
}

void generate_random_arr(int* a, int n)
{
	for (int i = 0; i < n; i++)
		a[i] = rand() % (AbsMaxVal * 2 + 1) + -1 * AbsMaxVal;
}