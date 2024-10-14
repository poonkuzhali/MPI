#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

#define N 2500000

int isPrime(int n) {
    if(n <= 1) return 0;
    if(n == 2) return 1;
    if(n%2 == 0) return 0;

    int sq_rt = (int)sqrt(n);
    for(int i=3; i<=sq_rt; i+=2) {
        if(n%i == 0) return 0;
    }

    return 1;
}

int main(int argc, char** argv) {
    int rank, size;
    int root = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // int N = atoi(argv[1]);

    double start = MPI_Wtime();

    int local_prime_count = 0;
    int capacity = 1000;
    int *prime_arr = (int*)malloc(capacity * sizeof(int));

    // Typically for cyclic distribution loop will look like
    // for(i=rank; i< N; i += size)
    // Since we are avoiding even numbers, multiplying by 2 at assignment and increment.
    for(int i=3+2*rank; i<N; i+=2*size) {
        if(isPrime(i)) {
            if(local_prime_count == capacity) {
                capacity = capacity * 2;
                prime_arr = (int*) realloc(prime_arr, capacity * sizeof(int));
            }

            prime_arr[local_prime_count] = i;
            local_prime_count += 1;
        }
    }

    int total = 0; 

    MPI_Reduce(&local_prime_count, &total, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
    total = total + 1; // accounting for 2.

    int *final_recv_count = (int*)malloc(size * sizeof(int));
    MPI_Gather(&local_prime_count, 1, MPI_INT, final_recv_count, 1, MPI_INT, root, MPI_COMM_WORLD);

    // int *final_primes = (int*)malloc(total * sizeof(int));  
    // int *displs = (int*)malloc(size * sizeof(int));

    if(rank == root) {
        printf("Total number of primes found: %d\n", total);

        printf("2 is a prime which we accounted for!\n");
        for(int i=0; i<size; i++)
            printf("Process: %d found %d prime numbers\n", i, final_recv_count[i]);

        // IGNORE-----displs for Gatherv
        // displs[0] = 0;
        // for(i=1; i<size; i++) {
        //     displs[i] = displs[i-1] + final_recv_count[i-1];
        // }
            
    }

    for(int i=0; i<local_prime_count; i++) {
        printf("Prime number: %d --- Process: %d\n", prime_arr[i], rank);
    }
     
    // IGNORE ---- if we want all primes to be received by root..
    // MPI_Gatherv(prime_arr, local_prime_count, MPI_INT, final_primes, final_recv_count, displs, MPI_INT, root, MPI_COMM_WORLD);
  
    // if(rank == root) {
    //     printf("\n Printing all primes: ------------------------- \n");

    //     // sort it and print - Find what gives 0.
    //     for(i=0; i<total; i++) {
    //         printf("Prime number: %d --- Process: %d\n", final_primes[i], rank);
    //     }
    // }


    double end = MPI_Wtime();

    if(rank == root) {
        printf("\n---------------------------------------------------\n");
        printf("Elapsed time: %f s\n", end-start);
    }

    free(prime_arr);
    free(final_recv_count);
    MPI_Finalize();
    return 0;
}