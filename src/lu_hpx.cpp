#include <iostream>
#include <iomanip>
#include <vector>

#include <mkl_lapack.h>
#include <mkl_lapacke.h>

#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/util.hpp>

#include "matrix.hpp"
#include "kernels.h"
#include "timer.hpp"

using hpx::lcos::future;
using hpx::when_all;

#define DEFAULT_NB 4
#define DEFAULT_SIZE 8

int IONE=1;
int ISEED[4] = {0,0,0,1};   /* initial seed for dlarnv() */

typedef future<void>  TileFuture;

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

  template<typename T>
  void operator()(T const &)
  {
    //CORE_dgetrf(m, n, &A(k*m,k*n), LDA, &IPIV[k*m+k*LDA], &info);
    CORE_dgetrf_incpiv(m, n, ib, &A(k*nb,k*nb), LDA, &IPIV[k*nb+k*LDA], &info);
  }

  Matrix<double> & A;
  int m,n,nb,ib,k,info,LDA;
  std::vector<int> & IPIV;
};

struct dtstrf_f
{

   dtstrf_f(  Matrix<double> & A_,
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

  template< typename T>
  void operator()(T const &)
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

  template<typename T>
  void operator()(T const &)
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

  template< typename T>
  void operator()(T const &)
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

    std::vector< Matrix<TileFuture> > Tiles;
    Tiles.reserve(TILES);

    Tiles.push_back(Matrix<TileFuture>(TILES+1,TILES+1,hpx::lcos::make_ready_future()));

    for(int k=0; k <TILES; k++)
    Tiles.push_back(Matrix<TileFuture>(TILES-k,TILES-k));

    for(int k=0; k < TILES; k++) {

    int km = (k==TILES-1) ? M - k*m : m;
    int kn = (k==TILES-1) ? N - k*n : n;

    //step 1
    Tiles[dst](0,0) = Tiles[src](1,1).then(dgetrf_f(A,km,kn,nb,ib,k,LDA,IPIV));
    //step 2
    for(int mm = k+1; mm < TILES; mm++) {

      int m_ = (mm==TILES-1) ? M -mm*m : m;

      Tiles[dst](mm-k,0) = when_all(Tiles[dst](mm-k-1,0),
                                    Tiles[src](mm-k+1,1)
                                    )
                           .then(dtstrf_f(A,L,m_,kn,nb,ib,k,mm,LDA,IPIV));
    }

    //step 3
    for(int nn = k+1; nn < TILES; nn++) {

    int n_ = (nn==TILES-1) ? N - nn*n : n;

    Tiles[dst](0,nn-k) = when_all(Tiles[dst](0,0),
                                  Tiles[src](1,nn-k+1)
                                  )
                        .then(dgessm_f(A,km,n_,nb,ib,k,nn,LDA,IPIV));
    }

    //step 4
    for(int nn = k+1; nn < TILES; nn++) {
            for(int mm=k+1; mm < TILES; mm++) {

                int m_ = (mm==TILES-1) ? M - mm*m : m;
                int n_ = (nn==TILES-1) ? N - nn*n : n;

                Tiles[dst](mm-k,nn-k) = when_all(  Tiles[dst](mm-k,0),
                                                   Tiles[dst](mm-k-1,nn-k),
                                                   Tiles[src](mm-k+1,nn-k+1)
                                                )
                                        .then(dssssm_f(A,L,m_,n_,nb,ib,k,mm,nn,LDA,IPIV));
            }
    }

        src ++;
        dst ++;
      }

    Tiles[src](0,0).get();

    return 0;
};

struct dgetrf_test
{
    dgetrf_test(std::size_t size_,std::size_t nb_, std::size_t ib_)
    :N(size_),LDA(N),LDL(ib_*(LDA/nb_)),LDAxN(LDA*N)
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


int hpx_main(boost::program_options::variables_map& vm){

    std::size_t size = vm["s"].as<std::size_t>();
    std::size_t nb = vm["b"].as<std::size_t>();
    std::size_t ib = (nb<40) ? nb : 40;

    dgetrf_test test(size,nb,ib);

    std::cout << "Running LU factorization" << std::endl;

    //LU factorization of the matrix A
    perform_benchmark(test, 10);

    return hpx::finalize(); // Handles HPX shutdown
}

int main(int argc, char* argv[])
{
    // Configure application-specific options
    boost::program_options::options_description
    desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
    ( "s"
    , boost::program_options::value<std::size_t>()->default_value(DEFAULT_SIZE)
    , "Size of matrix")
    ;
    desc_commandline.add_options()
    ( "b"
    , boost::program_options::value<std::size_t>()->default_value(DEFAULT_NB)
    , "Size of block")
    ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
