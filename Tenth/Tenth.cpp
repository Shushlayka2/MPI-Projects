#include <iostream>
#include <time.h>
#include "mpi.h"

#define ElementsPerProc 2
#define ArraySize 10
#define AbsMaxVal 1

int main()
{
	srand(time(NULL));
	int rank, world_size;
	int* x = (int*)malloc(sizeof(int) * ArraySize);
	int* A = NULL;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int MatrixRowCount = ElementsPerProc * world_size;
	int* global_result = NULL;
	int* local_result = (int*)calloc(ElementsPerProc, sizeof(int));

	if (rank == 0)
	{
		A = (int*)malloc(sizeof(int) * ArraySize * MatrixRowCount);
		global_result = (int*)malloc(sizeof(int) * MatrixRowCount);
		printf("Array: \n");
		for (int i = 0; i < ArraySize; i++)
		{
			x[i] = rand() % (AbsMaxVal * 2 + 1) + -1 * AbsMaxVal;
			printf("%i ", x[i]);
		}
		printf("\nMatrix: \n");
		for (int i = 0; i < ArraySize * MatrixRowCount; i++)
			A[i] = rand() % (AbsMaxVal * 2 + 1) + -1 * AbsMaxVal;
		for (int i = 0; i < MatrixRowCount; i++)
		{
			for (int j = 0; j < ArraySize; j++)
				printf("%i ", A[i * ArraySize + j]);
			printf("\n");
		}
	}

	MPI_Bcast(x, ArraySize, MPI_INT, 0, MPI_COMM_WORLD);
	int* sub_A = (int*)malloc(sizeof(int) * ElementsPerProc * ArraySize);
	MPI_Scatter(A, ElementsPerProc * ArraySize, MPI_INT, sub_A, ElementsPerProc * ArraySize, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < ElementsPerProc; i++)
	{
		for (int j = 0; j < ArraySize; j++)
			local_result[i] += sub_A[i * ArraySize + j] * x[j];
	}

	MPI_Gather(local_result, ElementsPerProc, MPI_INT, global_result, ElementsPerProc, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		printf("Result:\n");
		for (int i = 0; i < MatrixRowCount; i++)
			printf("%i ", global_result[i]);
		printf("\n");
	}

	MPI_Finalize();
}