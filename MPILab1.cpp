#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <Windows.h>
#include <time.h>

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(1251);
    srand(time(0));
    int ProcNum, ProcRank, RecvRank;
    bool flag = true;
    int message[5];
    for (int i = 0; i < 4; i++) {
        message[i] = rand() % 5 - 1;
    }

    int count = 0;
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    while (flag == true)
    {
        if (ProcRank == 0) {
            for (int i = 1; i < ProcNum; i++) {
                MPI_Recv(&message[i], 1, MPI_INT, MPI_ANY_SOURCE,
                    MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
                printf("Recived from %d thread message: %d\n", i, message[i]);
            }
            for (int i = 1; i < ProcNum; i++)
            {
                if (message[i] == -1)
                {
                    flag = 0;
                }
                else
                {
                    count++;
                }
            }
            for (int i = 1; i < ProcNum; i++) {
                MPI_Send(&flag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
        else
        {
            message[ProcRank] = rand() % 5 - 1;
            MPI_Send(&message[ProcRank], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&flag, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
        }
    }
    MPI_Finalize();
    printf("Value of sent message: %d\n", count);
    return 0;
}
