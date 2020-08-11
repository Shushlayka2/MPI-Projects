#include <iostream>
#include <time.h>
#include "mpi.h"

#define ElementsPerProc 5

typedef struct {
	float val;
	int   index;
} RankedVal;

void generate_random_arr(float* a, int n);

int main()
{
	srand(time(NULL));
	int rank, world_size;
	float* x = NULL;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	if (rank == 0)
	{
		int ArraySize = ElementsPerProc * world_size;
		x = (float*)malloc(sizeof(float) * ArraySize);
		generate_random_arr(x, ArraySize);
		printf("Generated array x: \n");
		for (int i = 0; i < ArraySize; i++)
			printf("%f ", x[i]);
		printf("\n");
	}

	RankedVal global_min;
	RankedVal local_min;
	local_min.index = rank * ElementsPerProc;
	float* sub_x = (float*)malloc(sizeof(float) * ElementsPerProc);

	MPI_Scatter(x, ElementsPerProc, MPI_FLOAT, sub_x, ElementsPerProc, MPI_FLOAT, 0, MPI_COMM_WORLD);

	local_min.val = sub_x[0];
	for (int i = 1; i < ElementsPerProc; i++)
	{
		if (local_min.val > sub_x[i])
		{
			local_min.val = sub_x[i];
			local_min.index = rank * ElementsPerProc + i;
		}
	}

	MPI_Reduce(&local_min, &global_min, 1, MPI_FLOAT_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		printf("Minimal value = %f\nIndex of this array item is %u\n", global_min.val, global_min.index);
	}

	MPI_Finalize();
}

void generate_random_arr(float* a, int n)
{
	for (int i = 0; i < n; i++)
		a[i] = (rand() / (float)RAND_MAX) * 2 - 1;
}