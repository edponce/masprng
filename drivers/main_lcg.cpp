#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "masprng.h"

#define YES 1
#define NO 0


int check_gen()
{
  int i, seed, param, scan_ret;
  int tempi, tempi2, correct, result = YES;
  const unsigned int fltmult = 1<<20, dblmult = 1<<30;
  double tempd;
 
  SPRNG *rng = selectType(1);

  seed = 985456376;
  param = 0;
  rng->init_rng(seed, param);

  correct = YES;

  for(i=0; i<200; i++)	
  {
    scan_ret = scanf("%d\n", &tempi);

    if(tempi != (tempi2 = rng->get_rn_int()))
      result = correct = NO;
  }
  
  if(correct == NO)
    printf("FAILED: Integer generator does not reproduce correct stream.\n\tArithmetic on this machine may not be compatible with this generator.\n");
  else
    printf("PASSED: Integer generator passed the reproducibility test\n");

  correct = YES;

  for(i=0; i<50; i++)	
  {
    scan_ret = scanf("%d\n", &tempi);
    tempd = (double) rng->get_rn_flt();
 
    if(abs((tempi>>11) - (int) (tempd*fltmult)) > 1 )
      result = correct = NO;
  }

  if(correct == NO)
    printf("FAILED: Float generator does not reproduce correct stream.\n\tArithmetic on this machine may not be compatible with this generator.\n");
  else
    printf("PASSED: Float generator passed the reproducibility test\n");

  correct = YES;

  for(i=0; i<50; i++)
  {
    scan_ret = scanf("%d", &tempi);
    tempd = rng->get_rn_dbl();

    if(abs((tempi>>1) - (int) (tempd*dblmult)) > 1 )
      result = correct = NO;
  }

  if(correct == NO)
    printf("FAILED: Double generator does not reproduce correct stream.\n\tArithmetic on this machine may not be compatible with this generator.\n");
  else
    printf("PASSED: Double generator passed the reproducibility test\n");

  return result;
}


int main()
{
  int result=YES;

  printf("Checking make_sprng_seed ...\n");

  if(check_gen() != YES)
    result = NO;

  if(result == YES)
    printf("\nResult:\t PASSED\n\n");
  else
    printf("\nResult:\t FAILED\n\n");

  return 0;
}

