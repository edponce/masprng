#ifdef SPRNG_MPI
#include <mpi.h>
#endif

#include "sprng.h"
#include <ctime>

using namespace std;

void get_proc_info_mpi(int *myid, int *nprocs)
{
#ifdef SPRNG_MPI
  MPI_Comm_rank(MPI_COMM_WORLD, myid);
  MPI_Comm_size(MPI_COMM_WORLD, nprocs);
#else
  *myid = 0;
  *nprocs = 1;
#endif
}

int make_new_seed_mpi(void)
{
#ifdef SPRNG_MPI
  unsigned int temp2;
  int myid, nprocs;
  MPI_Comm newcomm;
  
  MPI_Comm_dup(MPI_COMM_WORLD, &newcomm); /* create a temporary communicator */
  
  MPI_Comm_rank(newcomm, &myid);
  MPI_Comm_size(newcomm, &nprocs);

  if(myid == 0)
    temp2 = make_new_seed();
  
  MPI_Bcast(&temp2,1,MPI_UNSIGNED,0,newcomm);

  MPI_Comm_free(&newcomm);
  
  return temp2;
#else
  return make_new_seed();
#endif
}


#if 0
main()
{
  printf("%u\n", make_new_seed());
  printf("%u\n", make_new_seed());
}
#endif


/***********************************************************************************
* SPRNG (c) 2014 by Florida State University                                       *
*                                                                                  *
* SPRNG is licensed under a                                                        *
* Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. *
*                                                                                  *
* You should have received a copy of the license along with this                   *
* work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.           *
************************************************************************************/
