#ifndef _lcg_h
#define _lcg_h

/*! \class LCG 
 *  \brief Class for linear congruential RNG. 
 */
extern "C" {
class LCG : public Sprng
{
 public:

  LCG();
  int init_rng(int, int, int, int);
  ~LCG();
  LCG (const LCG &);
  LCG & operator= (const LCG &);

  int get_rn_int ();
  float get_rn_flt ();
  double get_rn_dbl ();
  int spawn_rng (int nspawned, Sprng ***newgens);
  int get_seed_rng ();
  int free_rng ();
  int pack_rng (char **buffer);
  int unpack_rng (char *packed);
  int print_rng ();

 private:

  int rng_type;  /*!< Unique ID for RNG */
#ifdef LONG64
  unsigned LONG64 seed;  /*!< Pair of seed values calculated using initial seed value */
  int init_seed;  /*!< Initial seed value */
  int prime;  /*!< Storage for a prime number */
  int prime_position;  /*!< Position of prime number */
  int prime_next;  /*!< Position of next prime number */
  char *gentype;  /*!< Contains string name of RNG */
  int parameter;  /*!< RNG parameter (not used for LCG) */
  unsigned LONG64 multiplier;  /*!< Array for multiplier values */

  /*! \brief Multiplication procedure using multiplier and seed parameters. 
   *
   *  The members used are seed, prime, and multiplier.
   */
  inline void multiply();

#else
  int seed[2];  /*!< Pair of seed values calculated using initial seed value */
  int init_seed;  /*!< Initial seed value */
  int prime;  /*!< Storage for a prime number */
  int prime_position;  /*!< Position of prime number */
  int prime_next;  /*!< Position of next prime number */
  char *gentype;  /*!< Contains string name of RNG */
  int parameter;  /*!< RNG parameter (not used for LCG) */
  int *multiplier;  /*!< Array for multiplier values */

  /*! \brief Multiplication procedure using multiplier and seed parameters. 
   *
   *  The members used are seed, prime, and multiplier.
   *
   *  \param m Array for multiplier values
   *  \param s Array for internal values
   *  \param res Array for internal values 
   */
  inline void multiply(int *, int *, int *);
  
#endif

  void advance_seed();
};
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
