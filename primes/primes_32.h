#ifndef __PRIMES_32_H
#define __PRIMES_32_H


const int MAXPRIME = 11863285;  /*!< sqrt(2)*2^23 + 2 */
const int MINPRIME = 3444;  /*!< sqrt(MAXPRIME) */
const int MAXPRIMEOFFSET = 779156;


int getprime_32(int, int *, int);


#endif // __PRIMES_32_H

