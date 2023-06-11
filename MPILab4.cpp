#include "mpi.h"
#include <iostream>
#include <time.h> // чтобы использовать функцию time()

typedef struct {
	double re;
	double im;
} Complex;

using namespace std;
#define MATR_N 3

Complex* multiply_matrix(Complex matr_a[MATR_N * MATR_N], Complex matr_b[MATR_N * MATR_N]) {
	Complex c[MATR_N * MATR_N] = { 0,0,0,0,0,0,0,0,0 };
	for (int i = 0; i < MATR_N; i++) {
		for (int j = 0; j < MATR_N; j++) {
			for (int num = 0; num < MATR_N; num++) {
				c[i * MATR_N + j].re += matr_a[i * MATR_N + num].re * matr_b[num * MATR_N + j].re -
					matr_a[i * MATR_N + num].im * matr_b[num * MATR_N + j].im;
				//printf("%lf \n", matr_a[i * MATR_N + num].re * matr_b[num * MATR_N + j].re -
					//matr_a[i * MATR_N + num].im * matr_b[num * MATR_N + j].im);
				c[i * MATR_N + j].im += matr_a[i * MATR_N + num].re * matr_b[num * MATR_N + j].im +
					matr_a[i * MATR_N + num].im * matr_b[num * MATR_N + j].re;
			}
		}
	}
	return c;
}

int main(int argc, char* argv[]) {
	int flag = 1;
	int a = 5;
	Complex matr_a[MATR_N * MATR_N];
	Complex matr_b[MATR_N * MATR_N];

	MPI_Init(&argc, &argv);
	MPI_Datatype my_struct;
	int len[2] = { 1, 1 };
	MPI_Aint pos[2] = { offsetof(Complex, re), offsetof(Complex, im) };
	MPI_Datatype typ[2] = { MPI_DOUBLE, MPI_DOUBLE };
	MPI_Type_create_struct(2, len, pos, typ, &my_struct);
	MPI_Type_commit(&my_struct);
	Complex* result = 0;
	int ProcNum, ProcRank;
	int count = 0;
	int matr_a_re[9] = { 2,1,3,0,2,0,4,0,4 };
	int matr_b_re[9] = { 0,0,1,1,3,9,0,-2,-3 };
	int matr_a_im[9] = { 3,-1,-1,3,0,1,2,4,0 };
	int matr_b_im[9] = { 7,0,1,-2,-5,-8,-4,0,0 };

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	while (flag == true) {
		if (ProcRank == 0) {
			for (int i = 1; i < ProcNum && count < a; i++, count++)
			{
				printf("count=%d\n", count);
				srand(time(NULL));
				for (int i = 0; i < MATR_N * MATR_N; i++) {
					matr_a[i].re = matr_a_re[i];
					matr_a[i].im = matr_a_im[i];
					matr_b[i].re = matr_b_re[i];
					matr_b[i].im = matr_b_im[i];
				}
				/*for (int i = 0; i < MATR_N * MATR_N; i++) {
					printf("%lf %lf ", matr_b[i].re, matr_b[i].im);
					if (i % 2 == 0 && i != 0)
						printf("\n");
				}*/
				MPI_Send(&flag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&matr_a, MATR_N * MATR_N, my_struct, i, 0, MPI_COMM_WORLD);
				MPI_Send(&matr_b, MATR_N * MATR_N, my_struct, i, 0, MPI_COMM_WORLD);
			}
			if (count == a) {
				for (int i = 1; i < ProcNum; i++)
				{
					flag = 0;
					MPI_Send(&flag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				}
			}
		}
		else {
			MPI_Recv(&flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (flag) {
				MPI_Recv(&matr_a, MATR_N * MATR_N, my_struct, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Recv(&matr_b, MATR_N * MATR_N, my_struct, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}

			result = multiply_matrix(matr_a, matr_b);
			if (ProcRank == 1) {
				for (int i = 0; i < MATR_N * MATR_N; i++) {
					printf("%lf %lf ", result[i].re, result[i].im);
					if (i % 2 == 0 && i != 0)
						printf("\n");
				}
			}

		}
	}
	MPI_Finalize();

	return 0;
}