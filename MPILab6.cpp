
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
    int mess = 1;
    int count = 0;
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    while (flag == true)
        if (ProcRank == 0) {
            MPI_Reduce(&message[0], &mess, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
            printf("Recived from thread message: %d \n", message[0]);
            if (message[0] == -1)
            {
                flag = 0;
            }
            else
            {
                count++;
            }
            for (int i = 1; i < ProcNum; i++) {
                MPI_Send(&flag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
        else
        {
            mess = rand() % 5 - 1;
            MPI_Reduce(&message[0], &mess, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
            MPI_Recv(&flag, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
        }
    MPI_Finalize();
    printf("Value of sent message: %d\n", count);
    return 0;
}


//#include "mpi.h"
//#include <stdio.h> 
//#include <time.h>
//#include <random>
//#include <iostream>
//#define NODE 4
//
//int main(int argc, char* argv[]) {
//    int rank, prime_rank, size;
//    int source, dest;
//
//    MPI_Comm comm_cart;
//    MPI_Status status;
//    MPI_Init(NULL, NULL);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//    MPI_Comm_rank(MPI_COMM_WORLD, &prime_rank);
//    rank = prime_rank;
//
//    MPI_Comm comm;
//    MPI_Comm graph, graph_2;
//    MPI_Group orig_group, mega_group;
//
//    MPI_Datatype my_arr;
//    MPI_Type_contiguous(size, MPI_INT, &my_arr);
//    MPI_Type_commit(&my_arr);
//
//    int* arr;
//    arr = (int*)calloc(size, sizeof(int));
//    int dims[] = { NODE };
//    int periods[] = { 1 };
//    int reorder = 0;
//
//    MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
//
//    int color = rank % 2;
//
//    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &comm);
//    MPI_Comm_group(comm, &mega_group);
//
//    if (prime_rank % 2 == 0) 
//    {
//        MPI_Group_rank(mega_group, &rank);
//        MPI_Cart_create(comm, 1, dims, periods, reorder, &comm_cart);
//        MPI_Cart_shift(comm_cart, 0, 1, &source, &dest);
//            arr[rank] = rank;
//            int result = 0;
//            MPI_Sendrecv(&rank, 1, MPI_INT, dest, 1, &result, 1, MPI_INT, source, 1, comm_cart, &status);
//            /*for (int i = 0; i < size / 2; i++) {
//                printf("rank = %d count = %d ", rank, result);
//            }*/
//        free(arr);
//
//    }
//
//    if (prime_rank % 2 == 1)
//    {
//        int index[] = { 1,2,3,6 };
//        int edges[] = { 3, 3, 3, 0,1,2 };
//        int count = 0;
//
//        MPI_Group_rank(mega_group, &rank);
//        MPI_Graph_create(comm, size / 2, index, edges, 0, &graph_2);
//        MPI_Graph_neighbors_count(graph_2, rank, &count);
//        int* neighbors = new int[count];
//        int* result = new int[count];
//        MPI_Graph_neighbors(graph_2, rank, count, neighbors);
//        for (int i = 0; i < count; i++) {
//            MPI_Sendrecv(&neighbors[i], 1, MPI_INT, neighbors[i], 0, &result[i], 1, MPI_INT, neighbors[i], 0, graph_2, &status);
//            printf("Rank %d result %d", neighbors[i], result[i]);
//        }
//    }
//
//    MPI_Comm_free(&comm);
//    MPI_Finalize();
//    return 0;
//}