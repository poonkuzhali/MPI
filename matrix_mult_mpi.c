#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(int argc, char **argv) 
{
    int i, j, rank, size;
    int r, c;
    r = atoi(argv[1]);
    c = atoi(argv[2]);
    int matrix[r][c];
    int vector[c];
    int result[r];

    int root = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == root) {
        for(i=0; i<r; i++) {
            for(j=0; j<c; j++) {
                matrix[i][j] = rand() % 10;
            }
        }

        for(i=0; i<c; i++) {
            vector[i] = rand() % 10;
        }

        printf("\n Generated matrix: \n");
        for(i=0; i<r; i++) {
            for(j=0; j<c; j++) {
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }

        printf("\n Generated vector: \n");
        for(i=0; i<c; i++) {
            printf("%d\n", vector[i]);
        }
    }

    MPI_Bcast(vector, c, MPI_INT, root, MPI_COMM_WORLD);

    int row_count = r/size;
    int rem = r % size;
    int rows;

    if(rank < rem) {
        rows = row_count + 1;
    } else {
        rows = row_count;
    }

    if(rows == 0) {
        MPI_Finalize();
        return 0;
    }

    int local_matrix[row_count][c];
    int local_result[row_count];

    MPI_Scatter(matrix, row_count * c, MPI_INT, local_matrix, row_count * c, MPI_INT, root, MPI_COMM_WORLD);

    for(i=0; i<row_count; i++) {
        local_result[i] = 0;
        for(j=0; j<c; j++) {
            local_result[i] += local_matrix[i][j] * vector[j];
        }
    }

    MPI_Gather(local_result, row_count, MPI_INT, result, row_count, MPI_INT, root, MPI_COMM_WORLD);

    if(rank == root) {
        printf("Final Result: \n");
        for(i=0; i<r; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");
    }
    
    MPI_Finalize();
    return 0;
}