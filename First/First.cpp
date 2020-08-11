#include <iostream>
#include <stdio.h>
#include "mpi.h"

using namespace std;

int main()
{
	int numtasks, rank;

	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("Process with rank %u out of %u processes\n", rank, numtasks);
	MPI_Finalize();
	return 0;
}