#include <iostream>
#include <time.h>
#include "mpi.h"
#include <fstream>

#define N 10000
#define IterationsCount 1000
#define Offset 9900

using namespace std;

void exec_a(int rank, ofstream& file);
void exec_b(int rank, ofstream& file);
void generate_random_arr(char* a, int n, int start_pos);

int main()
{
	ofstream pingPongFile, pingPingFile;
	pingPongFile.open("outputPingPong.txt");
	pingPingFile.open("outputPingPing.txt");
	srand(time(NULL));
	int rank;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	exec_a(rank, pingPongFile);
	//exec_b(rank, pingPingFile);
	MPI_Finalize();
	pingPongFile.close();
	pingPingFile.close();
}

void exec_a(int rank, ofstream& file)
{
	clock_t begin, end;
	double elapsed_secs;
	char* a = (char*)malloc(1);
	for (int n = N - Offset; n <= N; n++)
	{
		if (rank == 0)
		{
			a = (char*)realloc(a, n);
			if (n == N - Offset)
				generate_random_arr(a, n, 0);
			else
				generate_random_arr(a, n, n - 1);

			begin = clock();
			for (int i = 0; i < IterationsCount; i++)
			{
				MPI_Send(a, n, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
				MPI_Recv(a, n, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			end = clock();
			elapsed_secs = end - begin;
			file << elapsed_secs << endl;
		}
		else
		{
			a = (char*)realloc(a, n);
			for (int i = 0; i < IterationsCount; i++)
			{
				MPI_Recv(a, n, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(a, n, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
			}
		}
	}
	free(a);
}

void exec_b(int rank, ofstream& file)
{
	clock_t begin, end;
	double elapsed_secs;
	char* a = (char*)malloc(1);
	for (int n = N - Offset; n <= N; n++)
	{
		if (rank == 0)
		{
			a = (char*)realloc(a, n);
			if (n == N - Offset)
				generate_random_arr(a, n, 0);
			else
				generate_random_arr(a, n, n - 1);

			begin = clock();
			for (int i = 0; i < IterationsCount; i++)
			{
				MPI_Send(a, n, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
				MPI_Recv(a, n, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			end = clock();
			elapsed_secs = end - begin;
			file << elapsed_secs << endl;
		}
		else
		{
			a = (char*)realloc(a, n);
			for (int i = 0; i < IterationsCount; i++)
			{
				MPI_Send(a, n, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
				MPI_Recv(a, n, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}
	}
	free(a);
}

void generate_random_arr(char* a, int n, int start_pos)
{
	for (int i = start_pos; i < n; i++)
		a[i] = 'a' + rand() % 26;
}