/*
	these kernels are implementations
	of the plasma library
*/

#ifndef KERNELS_H
#define KERNELS_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

int  CORE_dtstrf(int M, int N, int IB, int NB,
                 double *U, int LDU,
                 double *A, int LDA,
                 double *L, int LDL,
                 int *IPIV, double *WORK,
                 int LDWORK, int *INFO);

int  CORE_dssssm(int M1, int N1, int M2, int N2, int K, int IB,
                       double *A1, int LDA1,
                       double *A2, int LDA2,
                 const double *L1, int LDL1,
                 const double *L2, int LDL2,
                 const int *IPIV);

int  CORE_dgetrf(int M, int N,
                 double *A, int LDA,
                 int *IPIV, int *INFO);

int  CORE_dgessm(int M, int N, int K, int IB,
                 const int *IPIV,
                 const double *L, int LDL,
                 double *A, int LDA);

int CORE_dgetrf_incpiv(int M, int N, int IB,
                       double *A, int LDA,
                       int *IPIV, int *INFO);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif

