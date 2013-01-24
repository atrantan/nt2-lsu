
/* Program usage:  mpiexec -n <procs> petsc_gemres [-help] [all PETSc options] */ 

static char help[] = "Solves a linear system in parallel with KSP.\n\
Input parameters include:\n\
  -random_exact_sol : use a random exact solution vector\n\
  -view_exact_sol   : write exact solution vector to stdout\n\
  -N <mesh_size>    : number of mesh points in larger\n\
  -maxits           :maximum number of iterations";

/*T
   Concepts: KSP^basic parallel example;
   Processors: n
T*/

/* 
  Include "petscksp.h" so that we can use KSP solvers.  Note that this file
  automatically includes:
     petscsys.h       - base PETSc routines   petscvec.h - vectors
     petscmat.h - matrices
     petscis.h     - index sets            petscksp.h - Krylov subspace methods
     petscviewer.h - viewers               petscpc.h  - preconditioners
*/
#include <iostream>

#include "petscksp.h"
#include "petsc_gmres_test.h"
#include "benchmark.hpp"

using namespace details;

#undef __FUNCT__
#define __FUNCT__ "main"

int main(int argc,char **args)
{   
    int nprocs;
  
    PetscInitialize(&argc,&args,(char *)0,help);    
    
    MPI_Comm_size(PETSC_COMM_WORLD, &nprocs);
    
    petsc_gmres_test my_test;

    details::benchmark_result<cycles_t> dv;
    details::perform_benchmark(my_test,1.0, dv);
    PetscPrintf(PETSC_COMM_WORLD,"%D %Fe+06\n",nprocs,dv.median/1e6);

//    my_test();
    
    KSP &ksp(my_test.ksp); 
    Vec &u(my_test.x);
    Vec &b(my_test.b);
    Mat &A(my_test.A); 
    
  /*
     Free work space.  All PETSc objects should be destroyed when they
     are no longer needed.
  */
  
    KSPDestroy(&ksp); 
    VecDestroy(&u);      
    VecDestroy(&b);     
    MatDestroy(&A); 

  /*
     Always call PetscFinalize() before exiting a program.
  */

    PetscFinalize();
    
  return 0;
}
