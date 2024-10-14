#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<time.h>

int main(int argc, char** argv) {
    int rank, size;
    int root = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(0));

    int l = atoi(argv[1]);
    int *p = (int*)malloc(l * sizeof(int));
    int *q = (int*)malloc(l * sizeof(int));

    if(rank == root) {
        for(int i=0; i<l; i++) {
            p[i] = rand() % 100;
            q[i] = rand() % 100;
        }

        for(int i=0; i<l; i++) {
            printf("%d ", p[i]);
        }
        printf("\n");

        for(int i=0; i<l; i++) {
            printf("%d ", q[i]);
        }
        printf("\n");
    }

    int local_p = 0;
    int local_q = 0;

    MPI_Scatter(p, 1, MPI_INT, &local_p, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Scatter(q, 1, MPI_INT, &local_q, 1, MPI_INT, root, MPI_COMM_WORLD);

    int p_sub_q = local_p - local_q;
    int sq = p_sub_q * p_sub_q;

    int total = 0;

    MPI_Reduce(&sq, &total, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);

    if(rank == root) {
        double euclidean_dist = sqrt(total);
        printf("Euclidean dist is %f \n", euclidean_dist);
    }

    free(p);
    free(q);
    MPI_Finalize();
    return 0;
}
