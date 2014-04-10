#include <unistd.h>
#include <iostream>
#include <iomanip>

#include "timer.hpp"
#include "matrix.hpp"

#include <mkl_lapacke.h>
#include <mkl_lapack.h>

#define DEFAULT_NB 4
#define DEFAULT_SIZE 8
#define DEFAULT_CORES 6

using namespace std;

int IONE=1;
int ISEED[4] = {0,0,0,1};   /* initial seed for dlarnv() */


template<typename T>
void print_array(int M, int N, T *A, int LDA) {
    std::cout << "----------" << std::endl;
    for(int i=0; i < M; i++) {
        for(int j=0; j < N; j++) {
            std::cout << std::setw(16) << A[i+j*LDA];
        }
        std::cout << std::endl;
     }
     std::cout << "----------" << std::endl;
}

struct dgetrf_test
{
    dgetrf_test(int size_)
    :N(size_),LDA(N),LDAxN(LDA*N)
    {
        // Allocate A
        A = (double *)malloc( LDA*N*( sizeof(double) ) );

        // Allocate L and IPIV
        IPIV = (int*)malloc( N*( sizeof(int) ) );
    }

    ~dgetrf_test()
    {
        free(A); free(IPIV);
    }

    void reset()
    {
        /* Initialize A Matrix */
        dlarnv(&IONE, ISEED, &LDAxN, A);
    }

    void operator()()
    {
       info = LAPACKE_dgetrf(LAPACK_COL_MAJOR, N, N, A, LDA, IPIV );
    }

    int N;
    int LDA;
    int info;
    int LDAxN;

    double *A;
    int *IPIV;

};


int main(int argc, char* argv[]) {

        int size = DEFAULT_SIZE;
        int nb = DEFAULT_NB;
        int cores = DEFAULT_CORES;
        char c;

        while ((c = getopt(argc, argv, "n:b:a:")) != -1)
        switch (c){
            case 'n':
                size = atoi(optarg);
            case 'b':
                nb = atoi(optarg);
                break;
            case 'a':
                cores = atoi(optarg);
                break;
            default:
                break;
        }

    dgetrf_test test(size);

    printf("-- MKL is initialized to run on %d cores. \n",cores);

    //LU factorization of the matrix A
    perform_benchmark(test, 5);

    return 0;
}

