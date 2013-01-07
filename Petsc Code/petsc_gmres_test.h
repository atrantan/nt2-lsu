#ifndef PETSC_GMRES_TEST_H
#define PETSC_GMRES_TEST_H

#include "petscksp.h"

class petsc_gmres_test
{
public:
  Vec            x0,x,b,u;  /* approx solution, RHS, exact solution */
  Mat            A;         /* linear system matrix */
  KSP            ksp;       /* linear solver context */
  PC             prec;
  PetscReal      norm;      /* norm of solution error */
  PetscReal      eps;    
  PetscInt       i,j,k,Istart,Iend,N,Nprocs,m,n,its,maxits;
  PetscScalar    v;
  PetscViewer    fd;
    
  petsc_gmres_test():eps(0.05),N(10),maxits(15)
  {
    char Mfilename[PETSC_MAX_PATH_LEN];
   
    PetscOptionsGetInt(PETSC_NULL,"-maxits",&maxits,PETSC_NULL);
    PetscOptionsGetString(PETSC_NULL, "-Mfilename",Mfilename,PETSC_MAX_PATH_LEN,PETSC_NULL);

    /*
       Open binary file.  Note that we use FILE_MODE_READ to indicate
       reading from this file.
    */
      
    PetscViewerBinaryOpen(PETSC_COMM_WORLD,Mfilename,FILE_MODE_READ,&fd);
            
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	  Compute the matrix and right-hand-side vector that define
	  the linear system, Ax = b.
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      
    // Initialization A
    
    MatCreate(PETSC_COMM_WORLD,&A);
    MatSetType(A,MATMPIAIJ);
    MatLoad(A,fd);
    PetscViewerDestroy(&fd);
      
    MatGetSize(A,0,&N);  
      
    m=N; n=N;
    
//    MatCreate(PETSC_COMM_WORLD,&A);
//    MatSetSizes(A,PETSC_DECIDE,PETSC_DECIDE,m,n);
//    MatSetUp(A);
//    MatSetFromOptions(A);
//
//    MatMPIDenseSetPreallocation(A,PETSC_NULL);
//    MatGetOwnershipRange(A,&Istart,&Iend);     
//
//    for (i=Istart; i<Iend; i++) { 
//      v = 2.1;
//      MatSetValues(A,1,&i,1,&i,&v,INSERT_VALUES);
//      
//      k=1;
//      if (i-k>=0) { j=i-k; v = -1.0-k*eps; MatSetValues(A,1,&i,1,&j,&v,INSERT_VALUES); }
//      if (i+k<=N-1) { j=i+k; v = -1.0+k*eps; MatSetValues(A,1,&i,1,&j,&v,INSERT_VALUES); }
//    }          

    MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY); 
    MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
			      Create parallel vectors.
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
    VecCreate(PETSC_COMM_WORLD,&u); 
    VecSetSizes(u,PETSC_DECIDE,m); 
    VecSetFromOptions(u); 
    VecDuplicate(u,&b);  
    VecDuplicate(b,&x);
    VecDuplicate(x,&x0);
    
    /* First element of x0 = 1 */ 
    v=1.0; i=0; VecSetValues(x0,1,&i,&v,INSERT_VALUES);
    VecAssemblyBegin(x0);
    VecAssemblyEnd(x0);
    VecCopy(x0,x);
    
    VecSet(u,1.0);     
    MatMult(A,u,b); 

    /*
      View the exact solution vector if desired
    */
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
		  Create the linear solver and set various options
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* 
      Create linear solver context
    */
    KSPCreate(PETSC_COMM_WORLD,&ksp);

    /* 
      Set operators. Here the matrix that defines the linear system
      also serves as the preconditioning matrix.
    */
    
    KSPGetPC(ksp,&prec);
    PCSetType(prec,PCNONE);
  
    KSPSetOperators(ksp,A,A,SAME_PRECONDITIONER);
    KSPSetNormType(ksp,KSP_NORM_UNPRECONDITIONED);

    KSPSetType(ksp,KSPGMRES);
    KSPGMRESSetOrthogonalization(ksp,KSPGMRESClassicalGramSchmidtOrthogonalization);
    KSPSetInitialGuessNonzero(ksp,PETSC_TRUE);

    KSPSetTolerances(ksp,1.e-6,PETSC_DEFAULT,PETSC_DEFAULT,maxits); 
    
    KSPSetFromOptions(ksp);

  };
  
  void operator()()
  {
    VecCopy(x0,x);
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
		    Solve the linear system
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    KSPSolve(ksp,b,x);
  };
};

#endif // PETSC_GEMRES_TEST_H
