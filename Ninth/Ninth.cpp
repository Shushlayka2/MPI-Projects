#include <iostream>
#include <time.h>
#include "mpi.h"

#define ElementsPerProc 2

typedef struct {
	float val;
	int   rank;
} RankedVal;

void generate_random_arr(float* a, int n);

int main()
{
	srand(time(NULL));
	int rank, world_size;
	float* x = NULL;
	float min_val = 1;
	int min_val_index = 0;
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

	float* sub_x = (float*)malloc(sizeof(float) * ElementsPerProc);
	RankedVal* local_ranked_arr = (RankedVal*)malloc(sizeof(RankedVal) * ElementsPerProc);
	RankedVal* global_ranked_arr = (RankedVal*)malloc(sizeof(RankedVal) * ElementsPerProc);

	MPI_Scatter(x, ElementsPerProc, MPI_FLOAT, sub_x, ElementsPerProc, MPI_FLOAT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < ElementsPerProc; i++)
	{
		local_ranked_arr[i].val = sub_x[i];
		local_ranked_arr[i].rank = rank;
	}

	MPI_Reduce(local_ranked_arr, global_ranked_arr, ElementsPerProc, MPI_FLOAT_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		for (int i = 0; i < ElementsPerProc; i++)
		{
			if (global_ranked_arr[i].val < min_val)
			{
				min_val = global_ranked_arr[i].val;
				min_val_index = ElementsPerProc * global_ranked_arr[i].rank + i;
			}
		}
		printf("Minimal value = %f\nIndex of this array item is %u", min_val, min_val_index);
	}

	MPI_Finalize();
}

void generate_random_arr(float* a, int n)
{
	for (int i = 0; i < n; i++)
		a[i] = (rand() / (float)RAND_MAX) * 2 - 1;
}