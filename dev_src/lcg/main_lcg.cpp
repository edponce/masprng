/*--- J. Ren: 2006 modified for version 4 */

/*--- Chris S.: June 1999  */
/*--- reads in, first an integer as a generator type and next elements from */
/*--- a data file */
/*--- checking Default C interface                                        ---*/
/*--- added 'int gtype' 'scanf("%d\n",  @gtype)' and gtype to all init_sprng */

//#define USE_MPI		/*	 Uncomment to test with MPI.  */

#ifdef USE_MPI
#include <mpi.h>
#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//#define SIMPLE_SPRNG
#include "sprng.h"

using namespace std;

#ifdef VERBOSE
#define report printf
#else
#define report ignore
#endif

#define PARAM SPRNG_DEFAULT

#define YES 1
#define NO 0

int gtype = 1; 

void ignore(const char *s, ...)
{
}


int check_gen()			/* Check generator with correct parameters   */
{
  int *gen;
  int i, size, seed, scan_ret;
  char *s;
  int tempi, tempi2, correct, result = YES;
  const unsigned int fltmult = 1<<20, dblmult = 1<<30;
  double tempd;
 
  seed = 985456376;
  gen = init_sprng(seed,PARAM, gtype); /* initialize generator                */

  if(gen == NULL)
  {
    result = NO;
    printf("FAILED: SPRNG was unable to initialize the generator\n");
  }
  
  /* ____________________ Check default generator ________________________   */
/*
  correct = YES;

  // check integer generator
  for(i=0; i<200; i++)	
  {
    scan_ret = scanf("%d\n", &tempi);

    if(tempi != (tempi2 = isprng()))
      result = correct = NO;
      printf("%d. %d %d\n", i, tempi, tempi2);
  }
  
  if(correct == NO)
    printf("FAILED: Integer generator does not reproduce correct stream.\n\tArithmetic on this machine may not be compatible with this generator.\n");
  else
    report("PASSED: Integer generator passed the reproducibility test\n");

  correct = YES;

  for(i=0; i<50; i++)	
  {
    scan_ret = scanf("%d\n", &tempi);
#ifdef USE_MPI
    tempd = (double) get_rn_flt_simple_mpi();
#else
    tempd = (double) get_rn_flt_simple();
#endif
 
    if(abs((tempi>>11) - (int) (tempd*fltmult)) > 1 )
    {
      result = correct = NO;
      printf("%d. %d %d\n", i, tempi>>11, (int)(tempd*fltmult));
    }
  }

  if(correct == NO)
    printf("FAILED: Float generator does not reproduce correct stream.\n\tArithmetic on this machine may not be compatible with this generator.\n");
  else
    report("PASSED: Float generator passed the reproducibility test\n");

*/
  correct = YES;

  for(i=0; i<50; i++)
  {
    scan_ret = scanf("%d", &tempi);
    tempd = sprng();

    if(abs((tempi>>1) - (int) (tempd*dblmult)) > 1 )
    {
      result = correct = NO;
      printf("%d. %d %d\n", i, tempi>>1, (int)(tempd*dblmult));
    }
  }

  if(correct == NO)
    printf("FAILED: Double generator does not reproduce correct stream.\n\tArithmetic on this machine may not be compatible with this generator.\n");
  else
    report("PASSED: Double generator passed the reproducibility test\n");

/*
  size = pack_sprng(&s);	

  if(size == 0)
  {
    result = NO;
    printf("FAILED: SPRNG was unable to pack the generator\n");
  }
  
  for(i=0; i<100; i++)	   
    isprng();

  gen = unpack_sprng(s, gtype);

  if(gen == NULL)
  {
    result = NO;
    printf("FAILED: SPRNG was unable to unpack the generator\n");
  }

  correct = YES;

  for(i=0; i<100; i++)
  {
    scan_ret = scanf("%d\n", &tempi);

    if(tempi != isprng())
      result = correct = NO;
  }

  if(correct == NO)
    printf("FAILED: Generator does not reproduce correct stream after packing.\n\tProbably error in packing/unpacking\n");
  else
    report("PASSED: Generator packs and unpacks correctly.\n");
*/

  return result;
}


/* Check if generator meets specifications in handling errors           */
int check_errors()
{
  int *gen1;
  int i, scan_ret;
  int tempi, correct, result = YES;
  char s[MAX_PACKED_LENGTH];
  
  /* ____________________ Unpack invalid string _____________________        */

#ifndef CREATE_DATA
  memset(s,0,MAX_PACKED_LENGTH); /* set string to 0's                        */

  fprintf(stderr,"Expect SPRNG ERROR: packed string invalid\n");
  
  gen1 = unpack_sprng(s, gtype);

  if(gen1 != NULL)	    /* NULL should be returned for invalid string    */
    printf("FAILED: Generator unpacks invalid string\n");
  else
    report("PASSED: Generator detected invalid string while unpacking\n");
#endif

  correct = YES;

  for(i=0; i<100; i++)		/* check packing/unpacking                   */
  {
    scan_ret = scanf("%d\n", &tempi);
    
    if(tempi != isprng())
      result = correct = NO;
  }

  if(correct == NO)
    printf("FAILED: Generator does not maintain original stream when unpacked stream is invalid.\n");
  else
    report("PASSED: Generator maintains original stream when unpacked stream is invalid.\n");

  return result;
}

#ifdef USE_MPI
int check_mpi_seed(unsigned int seed)
{
  int nprocs, myid, result = YES, i, tag=0;
  MPI_Status status;
  unsigned int temp;
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if(myid != 0)
    MPI_Send(&seed, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD);
  else
    for(i=1; i<nprocs; i++)
    {
      MPI_Recv(&temp,1, MPI_UNSIGNED, i, tag, MPI_COMM_WORLD, &status);
      if(temp != seed)
	result = NO;
    }
  
  if(result == NO)
    printf("FAILED: Seeds returned by make_seed differ on different processors\n");
  else
    report("PASSED: Seeds returned my make_seed on all processes are equal.\n");
  
  return result;
}
#endif


int main(int argc, char *argv[])
{
  int result=YES;
  unsigned int seed1, seed2;

  report("Checking make_sprng_seed ...  ");

#ifdef USE_MPI
  int myid;
  MPI_Init(&argc, &argv);
#endif
  seed1 = make_sprng_seed();

#ifdef USE_MPI
  result = check_mpi_seed(seed1);
#endif

  seed2 = make_sprng_seed();
#ifdef USE_MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  if(myid==0)
  {
#endif
  if(seed1 != seed2)
    report("  ... Checked make_sprng_seed\n");
  else
  {
    result = NO;
    printf("\nERROR: make_sprng_seed does not return unique seeds\n");
  }
  //  scanf("%d\n", &gtype);  /* reading in a generator type */
  
  if(check_gen() != YES)
    result = NO;

  //if(check_errors() != YES)
  //  result = NO;

  if(result == YES)
    printf("\nResult:\t PASSED\n\n");
  else
    printf("\nResult:\t FAILED\n\n");

#ifdef USE_MPI
}
  MPI_Finalize();
#endif
}

