#include <iostream>
#include <iomanip>
#include <vector>

#include <mkl_lapack.h>
#include <mkl_lapacke.h>

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>
#include "matrix.hpp"
#include "kernels.h"
#include "timer.hpp"

using namespace tbb;  
using namespace tbb::flow;

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

struct empty_body
{
    void operator()( continue_msg ) const
    {}
};

struct dgetrf_f
{

   dgetrf_f(Matrix<double> & A_,
    int m_,
    int n_,
    int nb_,
    int ib_,
    int k_,
    int LDA_,
    std::vector<int> & IPIV_
    )
  :A(A_),m(m_),n(n_),nb(nb_),ib(ib_),k(k_),LDA(LDA_),IPIV(IPIV_)
  {}

  void operator()(const continue_msg&)
  {
    CORE_dgetrf_incpiv(m, n, ib, &A(k*nb,k*nb), LDA, &IPIV[k*nb+k*LDA], &info);
  }

  Matrix<double> & A;
  int m,n,nb,ib,k,info,LDA;
  std::vector<int> & IPIV;
};

struct dtstrf_f
{

   dtstrf_f(    Matrix<double> & A_,
    Matrix<double> & L_,
    int m_,
    int n_,
    int nb_,
    int ib_,
    int k_,
    int mm_,
    int LDA_,
    std::vector<int> & IPIV_
    )
  :A(A_),L(L_),m(m_),n(n_),nb(nb_),ib(ib_),k(k_),mm(mm_),LDA(LDA_),
   IPIV(IPIV_)
  {}


  void operator()(const continue_msg&)
  {
    std::vector<double> work(m*n);
    CORE_dtstrf(m, n, ib, n, &A(k*nb,k*nb), LDA, &A(mm*nb,k*nb), LDA,
                &L(mm*ib,k*nb), nb, &IPIV[mm*nb+k*LDA], &work[0], nb, &info);
  }

  Matrix<double> & A, & L;
  int m,n,nb,ib,k,mm,info,LDA;
  std::vector<int> & IPIV;
};

struct dgessm_f
{

   dgessm_f(    Matrix<double> & A_,
    int m_,
    int n_,
    int nb_,
    int ib_,
    int k_,
    int nn_,
    int LDA_,
    std::vector<int> & IPIV_
    )
  :A(A_),m(m_),n(n_),nb(nb_),ib(ib_),k(k_),nn(nn_),LDA(LDA_),
   IPIV(IPIV_)
  {}


  void operator()(const continue_msg&)
  {
   CORE_dgessm(m, n, m, ib, &IPIV[k*nb+k*LDA], &A(k*nb,k*nb), LDA,
     &A(k*nb,nn*nb), LDA);
  }

  Matrix<double> & A;
  int m,n,nb,ib,k,nn,LDA;
  std::vector<int> & IPIV;
};


struct dssssm_f
{
   dssssm_f(    Matrix<double> & A_,
    Matrix<double> & L_,
    int m_,
    int n_,
    int nb_,
    int ib_,
    int k_,
    int mm_,
    int nn_,
    int LDA_,
    std::vector<int> & IPIV_
    )
  :A(A_),L(L_),m(m_),n(n_),nb(nb_),ib(ib_),k(k_),mm(mm_),nn(nn_),LDA(LDA_),IPIV(IPIV_)
  {}


  void operator()(const continue_msg&)
  {
   CORE_dssssm(nb, n, m, n, nb, ib, &A(k*nb,nn*nb), LDA, &A(mm*nb,nn*nb), LDA,
               &L(mm*ib,k*nb), m, &A(mm*nb,k*nb), LDA, &IPIV[mm*nb+k*LDA]);
  }

  Matrix<double> & A, & L;
  int m,n,nb,ib,k,mm,nn,LDA;
  std::vector<int> & IPIV;
};

int dgetrf(int M, int N, int nb, int ib, Matrix<double>& A, int LDA, Matrix<double>& L, std::vector<int>& IPIV) {

    int TILES = M/nb;
    int m = M/TILES;
    int n = N/TILES;

    int src(0), dst(1);

    graph g;
    continue_node<continue_msg> start(g,empty_body());
    std::vector< Matrix< continue_node<continue_msg> * > > Tiles;
    Tiles.reserve(TILES);

    Tiles.push_back(Matrix< continue_node<continue_msg> * >(TILES+1,TILES+1, & start));

    for(int k=0; k <TILES; k++)
    Tiles.push_back(Matrix< continue_node<continue_msg> * >(TILES-k,TILES-k));

    for(int k=0; k < TILES; k++) {

    int km = (k==TILES-1) ? M - k*m : m;
    int kn = (k==TILES-1) ? N - k*n : n;

    //step 1
    Tiles[dst](0,0) = new continue_node<continue_msg>( g, dgetrf_f(A,km,kn,nb,ib,k,LDA,IPIV) );
    make_edge(*(Tiles[src](1,1)),*(Tiles[dst](0,0)) );   

    //step 2
    for(int mm = k+1; mm < TILES; mm++) {

      int m_ = (mm==TILES-1) ? M -mm*m : m;

      Tiles[dst](mm-k,0) = new continue_node<continue_msg>( g, dtstrf_f(A,L,m_,kn,nb,ib,k,mm,LDA,IPIV)) ;
      make_edge(*(Tiles[dst](mm-k-1,0)), *(Tiles[dst](mm-k,0)) );
      make_edge(*(Tiles[src](mm-k+1,1)), *(Tiles[dst](mm-k,0)) );
    }

    //step 3
    for(int nn = k+1; nn < TILES; nn++) {

    int n_ = (nn==TILES-1) ? N - nn*n : n;

    Tiles[dst](0,nn-k) = new continue_node<continue_msg>( g, dgessm_f(A,km,n_,nb,ib,k,nn,LDA,IPIV)) ;
    make_edge(*(Tiles[dst](0,0)), *(Tiles[dst](0,nn-k)));
    make_edge(*(Tiles[src](1,nn-k+1)), *(Tiles[dst](0,nn-k)));
    }

    //step 4
    for(int nn = k+1; nn < TILES; nn++) {
            for(int mm=k+1; mm < TILES; mm++) {

                int m_ = (mm==TILES-1) ? M - mm*m : m;
                int n_ = (nn==TILES-1) ? N - nn*n : n;

                Tiles[dst](mm-k,nn-k) = new continue_node<continue_msg>( g, dssssm_f(A,L,m_,n_,nb,ib,k,mm,nn,LDA,IPIV));
                make_edge(*(Tiles[dst](mm-k,0)), *(Tiles[dst](mm-k,nn-k)));
                make_edge(*(Tiles[dst](mm-k-1,nn-k)), *(Tiles[dst](mm-k,nn-k)));
                make_edge(*(Tiles[src](mm-k+1,nn-k+1)), *(Tiles[dst](mm-k,nn-k)));
            }
    }

      src ++;
      dst ++;
    }

    start.try_put(continue_msg()); 
    g.wait_for_all();

    
    Tiles.push_back(Matrix< continue_node<continue_msg> * >(TILES+1,TILES+1, & start));

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
    ,A(LDA,N),L(LDL,N),IPIV(N*LDA/nb_)
    {}


    void reset()
    {
        /* Initialize A Matrix */
        dlarnv(&IONE, ISEED, &LDAxN, &A(0,0));
    }

    void operator()()
    {
       info = dgetrf(N, N, nb, ib, A, LDA, L, IPIV);
    }

    int N;
    int LDA;
    int LDL;
    int info;
    int LDAxN;
    int nb;
    int ib;

    Matrix<double> A;
    Matrix<double> L;
    std::vector<int> IPIV;

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

    tbb::task_scheduler_init init(cores);

    dgetrf_test test(size,nb,ib);

    printf("--TBB is initialized to run on %d cores. \n",cores);

    //LU factorization of the matrix A
    perform_benchmark(test, 10);

    // test.reset();
    // test();
    // print_matrix(size, size, &(test.A(0,0)), size);

    return 0;
}
