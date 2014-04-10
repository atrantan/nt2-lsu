#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdio>

#include <mkl_lapack.h>
#include <mkl_lapacke.h>

#include "kernels.h"
#include "timer.hpp"
#include "matrix.hpp"

#define DEFAULT_NB 4
#define DEFAULT_SIZE 8
#define DEFAULT_CORES 6

int IONE=1;
int ISEED[4] = {0,0,0,1};   /* initial seed for dlarnv() */

void print_matrix(int M, int N, double *A, int LDA) {
  std::cout << "----------" << std::endl;
  for(int j=0; j < N; j++) {
    for(int i=0; i < M; i++) {
      std::cout << std::setw(16) << A[i*LDA+j];
    }
    std::cout << std::endl;
  }
  std::cout << "----------" << std::endl;
}

struct dgetrf_f
{

   dgetrf_f(
    double * A_,
    int m_,
    int n_,
    int nb_,
    int ib_,
    int k_,
    int LDA_,
    int * IPIV_
    )
  :A(A_),m(m_),n(n_),nb(nb_),ib(ib_),k(k_),LDA(LDA_),IPIV(IPIV_)
  {}

  int operator()()
  {
    printf("dgetrf %d %d at %p\n",k,k,A);
    CORE_dgetrf_incpiv(m, n, ib, &A[k*nb + k*nb*LDA], LDA, &IPIV[k*nb+k*LDA], &info);
    return 0;
  }

  double * A;
  int m,n,nb,ib,k,info,LDA;
  int * IPIV;
};

struct dtstrf_f
{

   dtstrf_f(    
    double * A_,
    double * L_,
    int m_,
    int n_,
    int nb_,
    int ib_,
    int k_,
    int mm_,
    int LDA_,
    int LDL_,
    int * IPIV_
    )
  :A(A_),L(L_),m(m_),n(n_),nb(nb_),ib(ib_),k(k_),mm(mm_),LDA(LDA_),LDL(LDL_),
   IPIV(IPIV_)
  {}


  int operator()()
  {
    std::vector<double> work(m*n);
    printf("dtstrf %d %d at %p\n",mm,k,A);
    CORE_dtstrf(m, n, ib, n, &A[k*nb + k*nb*LDA], LDA, &A[mm*nb + k*nb*LDA], LDA,
                &L[mm*ib + k*nb*LDL], nb, &IPIV[mm*nb+k*LDA], &work[0], nb, &info);
    return 0;
  }

  double * A, * L;
  int m,n,nb,ib,k,mm,info,LDA,LDL;
  int * IPIV;
};

struct dgessm_f
{

   dgessm_f(
    double * A_,
    int m_,
    int n_,
    int nb_,
    int ib_,
    int k_,
    int nn_,
    int LDA_,
    int * IPIV_
    )
  :A(A_),m(m_),n(n_),nb(nb_),ib(ib_),k(k_),nn(nn_),LDA(LDA_),
   IPIV(IPIV_)
  {}


  int operator()()
  {
   printf("dgessm %d %d at %p\n",k,nn,A);
   CORE_dgessm(m, n, m, ib, &IPIV[k*nb+k*LDA], &A[k*nb + k*nb*LDA], LDA,
     &A[k*nb + nn*nb*LDA], LDA);
   return 0;
  }

  double * A;
  int m,n,nb,ib,k,nn,LDA;
  int * IPIV;
};


struct dssssm_f
{
   dssssm_f(    
    double * A_,
    double * L_,
    int m_,
    int n_,
    int nb_,
    int ib_,
    int k_,
    int mm_,
    int nn_,
    int LDA_,
    int LDL_,
    int * IPIV_
    )
  :A(A_),L(L_),m(m_),n(n_),nb(nb_),ib(ib_),k(k_),mm(mm_),nn(nn_),LDA(LDA_),LDL(LDL_),IPIV(IPIV_)
  {}


  int operator()()
  {
   printf("dssssm %d %d at %p\n",mm,nn,A);
   CORE_dssssm(nb, n, m, n, nb, ib, &A[k*nb + nn*nb*LDA], LDA, &A[mm*nb + nn*nb*LDA], LDA,
               &L[mm*ib +  k*nb*LDL ], m, &A[mm*nb + k*nb*LDA], LDA, &IPIV[mm*nb+k*LDA]);
   return 0;
  }

  double * A, * L;
  int m,n,nb,ib,k,mm,nn,LDA,LDL;
  int * IPIV;
};

int dgetrf(int M, int N, int nb, int ib, double * A, int LDA, double * L, int LDL, int * IPIV) {

    int TILES = M/nb;
    int m = M/TILES;
    int n = N/TILES;

    int src(0), dst(1);


    int * in1_dep; 
    int * in2_dep;
    int * in3_dep;
    int * out_dep;

    int start(1);

    std::vector< Matrix<int *> > Tiles;
    Tiles.reserve(TILES);

    Tiles.push_back(Matrix< int * >(TILES+1,TILES+1, & start));

    for(int k=0; k <TILES; k++)
    {
      Tiles.push_back(Matrix< int * >(TILES-k,TILES-k));

      std::size_t sizemax = (TILES-k)*(TILES-k);

      for(int i=0; i <sizemax; i++)
      {
        (Tiles[k+1].data)[i] = new int;
      }

    }

    int* start_dep(&start);

    #pragma omp task firstprivate(start_dep) out(start_dep)
    {
          *start_dep = 0;
    }


    for(int k=0; k < TILES; k++) {

    int km = (k==TILES-1) ? M - k*m : m;
    int kn = (k==TILES-1) ? N - k*n : n;

    in1_dep = Tiles[src](1,1);
    out_dep =Tiles[dst](0,0);

    //step 1
    #pragma omp task \
       firstprivate(in1_dep,out_dep) \
       in ( in1_dep ) \
       out( out_dep )
       {
        *out_dep = dgetrf_f(A,km,kn,nb,ib,k,LDA,IPIV)();
       }


    //step 2
    for(int mm = k+1; mm < TILES; mm++) {

      int m_ = (mm==TILES-1) ? M -mm*m : m;

     in1_dep = Tiles[dst](mm-k-1,0); 
     in2_dep = Tiles[src](mm-k+1,1);
     out_dep = Tiles[dst](mm-k,0);


      #pragma omp task \
        firstprivate(in1_dep,in2_dep,out_dep) \
        in ( in1_dep , in2_dep ) \
        out( out_dep )
        {
          *out_dep =  dtstrf_f(A,L,m_,kn,nb,ib,k,mm,LDA,LDL,IPIV)();
        }
    }

    //step 3
    for(int nn = k+1; nn < TILES; nn++) {

    int n_ = (nn==TILES-1) ? N - nn*n : n;

    in1_dep = Tiles[dst](0,0); 
    in2_dep = Tiles[src](1,nn-k+1);
    out_dep = Tiles[dst](0,nn-k);

      #pragma omp task \
        firstprivate(in1_dep,in2_dep,out_dep) \
        in ( in1_dep , in2_dep ) \
        out( out_dep )
       {
        *out_dep = dgessm_f(A,km,n_,nb,ib,k,nn,LDA,IPIV)();
       }

    }

    //step 4
    for(int nn = k+1; nn < TILES; nn++) {
            for(int mm=k+1; mm < TILES; mm++) {

                int m_ = (mm==TILES-1) ? M - mm*m : m;
                int n_ = (nn==TILES-1) ? N - nn*n : n;

                in1_dep= Tiles[dst](mm-k,0) ; 
                in2_dep= Tiles[dst](mm-k-1,nn-k) ;
                in3_dep= Tiles[src](mm-k+1,nn-k+1) ;
                out_dep= Tiles[dst](mm-k,nn-k) ;

                 #pragma omp task \
                  firstprivate(in1_dep,in2_dep, in3_dep, out_dep)\
                  in ( in1_dep , in2_dep , in3_dep) \
                  out( out_dep )
                    {
                      *out_dep = dssssm_f(A,L,m_,n_,nb,ib,k,mm,nn,LDA,LDL,IPIV)();
                    }
            }
    }

      src ++;
      dst ++;
    }

    #pragma omp taskwait

    for(int k=0; k <TILES; k++)
    {
        std::size_t sizemax = (TILES-k)*(TILES-k);

        for(int i=0; i <sizemax; i++)
        {
          delete( (Tiles[k+1].data)[i] );
        }
    }

    return 0;
};

struct dgetrf_test
{
    dgetrf_test(std::size_t size_,std::size_t nb_, std::size_t ib_)
    :N(size_),LDA(N),LDL(ib_*(LDA/nb_)),LDAxN(LDA*N),nb(nb_),ib(ib_)
    {
       A = new double[LDA*N];
       L = new double[LDL*N];
       IPIV = new int[N*LDA/nb_];
    }

    ~dgetrf_test()
    {
      delete[] A; delete[] L; delete[] IPIV;
    }


    void reset()
    {
        /* Initialize A Matrix */
        dlarnv(&IONE, ISEED, &LDAxN, &A[0]);
    }

    void operator()()
    {
       info = dgetrf(N, N, nb, ib, A, LDA, L, LDL, IPIV);
    }

    int N;
    int LDA;
    int LDL;
    int info;
    int LDAxN;
    int nb;
    int ib;

    double * A;
    double * L;
    int * IPIV;

};


int main(int argc, char* argv[]) {

        int size = DEFAULT_SIZE;
        int nb = DEFAULT_NB;
        std::size_t ib = (nb<40) ? nb : 40;

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

    dgetrf_test test(size,nb,ib);

    printf("--OMPSS is initialized to run on %d cores. \n",cores);

    //LU factorization of the matrix A
    // perform_benchmark(test, 5);

    test.reset();
    print_matrix(size, size, &(test.A[0]), size);
    test();
    print_matrix(size, size, &(test.A[0]), size);

    return 0;
}
