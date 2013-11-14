

#include <iostream>
#include <iomanip>

#include "timer.hpp"
#include "matrix.hpp"

#include <mkl_lapacke.h>
#include <mkl_lapack.h>

#include <plasma.h>

#define DEFAULT_NB 4
#define DEFAULT_SIZE 8
#define DEFAULT_CORES 6

using namespace std;

int SIZE;
int NB;
int CORES;

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
    dgetrf_test()
    :N(SIZE),LDA(N),LDAxN(LDA*N)
    {
        // Allocate A
        A = (double *)malloc(LDA*N*(sizeof*A));

        // Allocate L and IPIV
        info = PLASMA_Alloc_Workspace_dgetrf_incpiv(N, N, &L, &IPIV);
    }

    ~dgetrf_test()
    {
        free(A); free(IPIV); free(L);
    }

    void reset()
    {
        /* Initialize A Matrix */
        dlarnv(&IONE, ISEED, &LDAxN, A);
    }

    void operator()()
    {
       info = PLASMA_dgetrf_incpiv(N, N, A, LDA, L, IPIV);
    }

    int N;
    int LDA;
    int info;
    int LDAxN;

    double *A;
    PLASMA_desc *L;
    int *IPIV;

};


int main(int argc, char* argv[]) {

        SIZE = DEFAULT_SIZE;
        NB = DEFAULT_NB;
        CORES = DEFAULT_CORES;
        char c;

        while ((c = getopt(argc, argv, "n:b:a:")) != -1)
        switch (c){
            case 'n':
                SIZE = atoi(optarg);
            case 'b':
                NB = atoi(optarg);
                break;
            case 'a':
                CORES = atoi(optarg);
                break;
            default:
                break;
        }

    //Plasma Initialize
    PLASMA_Init(CORES);

    //PLASMA_Disable(PLASMA_AUTOTUNING);
    //PLASMA_Set(PLASMA_TILE_SIZE, NB);
    //PLASMA_Set(PLASMA_INNER_BLOCK_SIZE, NB);

    PLASMA_Set(PLASMA_SCHEDULING_MODE, PLASMA_DYNAMIC_SCHEDULING);

    dgetrf_test test;

    printf("-- PLASMA is initialized to run on %d cores. \n",CORES);

    //LU factorization of the matrix A
    perform_benchmark(test, 10);

    PLASMA_Finalize();

    return 0;
}

