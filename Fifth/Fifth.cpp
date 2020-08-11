#include <iostream>
#include <time.h>
#include "mpi.h"

#define ElementsPerProc 2

int main()
{
	srand(time(NULL));
	int rank, world_size;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int N = ElementsPerProc * (world_size - 1);
	if (rank == 0)
	{
		int* A = (int*)malloc(sizeof(int) * N * N);
		for (int i = 0; i < N * N; i++)
			A[i] = rand();
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				printf("%u ", A[i * N + j]);
			}
			printf("\n");
		}

		int* x = (int*)malloc(sizeof(int) * N);
		for (int i = 1; i < world_size; i++)
			MPI_Send(A + (i - 1) * ElementsPerProc * N, ElementsPerProc * N, MPI_INT, i, 0, MPI_COMM_WORLD);
		for (int i = 1; i < world_size; i++)
			MPI_Recv(x + (i - 1) * ElementsPerProc, ElementsPerProc, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (int i = 0; i < N; i++)
			printf("%u ", x[i]);
		free(A);
		free(x);
	}
	else
	{
		int length;
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &length);
		int* A = (int*)malloc(sizeof(int) * length);
		int* x = (int*)malloc(sizeof(int) * ElementsPerProc);
		MPI_Recv(A, length, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (int i = 0; i < ElementsPerProc; i++)
			x[i] = A[i * N + (rank - 1) * ElementsPerProc + i];
		MPI_Send(x, ElementsPerProc, MPI_INT, 0, 0, MPI_COMM_WORLD);
		free(A);
		free(x);
	}
	MPI_Finalize();
}