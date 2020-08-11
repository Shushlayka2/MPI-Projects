#include <iostream>
#include "mpi.h"

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
	if (rank == 0)
	{
		int a[10];
		for (int i = 0; i < 10; i++)
			a[i] = rand();
		MPI_Send(a, 10, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("Process 0 sent array of nums: ");
		for (int elem : a)
			printf("%u ", elem);
		printf("\n");
	}
	else if (rank == 1)
	{
		int a[10];
		MPI_Recv(a, 10, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		printf("Process 1 received array of nums: ");
		for(int elem : a)
			printf("%u ", elem);
		printf("\n");
	}
	MPI_Finalize();
}