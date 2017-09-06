#ifndef _sprng_h_
#define _sprng_h_

#define DEFAULT_RNG_TYPE SPRNG_LFG

#define SPRNG_LFG   0
#define SPRNG_LCG   1
#define SPRNG_LCG64 2
#define SPRNG_CMRG  3
#define SPRNG_MLFG  4
#define SPRNG_PMLCG 5

#define SPRNG_DEFAULT 0
#define CRAYLCG 0
#define DRAND48 1
#define FISH1   2
#define FISH2   3
#define FISH3   4
#define FISH4   5
#define FISH5   6
#define LECU1   0
#define LECU2   1
#define LECU3   2
#define LAG1279  0
#define LAG17    1
#define LAG31    2
#define LAG55    3
#define LAG63    4
#define LAG127   5
#define LAG521   6
#define LAG521B  7
#define LAG607   8
#define LAG607B  9
#define LAG1279B 10

#define MAX_PACKED_LENGTH 24000

#ifdef USE_MPI
#define MPINAME(A) A ## _mpi
#else
#define MPINAME(A) A
#endif

#define make_sprng_seed MPINAME(make_new_seed)

#if defined(SIMPLE_SPRNG)

#define pack_sprng pack_rng_simple
#define unpack_sprng unpack_rng_simple
#define isprng  MPINAME(get_rn_int_simple)
#define init_sprng MPINAME(init_rng_simple) 
#define print_sprng print_rng_simple

#ifdef FLOAT_GEN
#define sprng  MPINAME(get_rn_flt_simple)
#else
#define sprng  MPINAME(get_rn_dbl_simple)
#endif

#else

#define free_sprng free_rng
#define pack_sprng pack_rng
#define unpack_sprng unpack_rng
#define isprng  get_rn_int
#define spawn_sprng spawn_rng
#define init_sprng init_rng 
#define print_sprng print_rng

#ifdef FLOAT_GEN
#define sprng  get_rn_flt
#else
#define sprng  get_rn_dbl
#endif

#endif

/*! \class Sprng
 *  \brief Class for RNG types. 
 *
 *  All methods are pure virtual, so each class for RNG types needs to define at least these methods.
 */
class Sprng
{
 public:

  virtual ~Sprng(){};
  virtual int init_rng (int, int, int, int) = 0;
  virtual int get_rn_int () = 0;
  virtual float get_rn_flt () = 0;
  virtual double get_rn_dbl () = 0;
  virtual int spawn_rng (int nspawned, Sprng ***newgens) = 0;
  virtual int get_seed_rng () = 0;
  virtual int free_rng () = 0;
  virtual int pack_rng (char **buffer) = 0;
  virtual int print_rng () = 0;
  virtual int unpack_rng(char *packed) = 0;
};

/*! \brief Generates a new integral seed value based on the local time.
 *  \return Integral seed value
 */ 
int make_new_seed ();

/*! \brief (MPI) Generates a new integral seed value based on the local time.
 *  \return Integral seed value
 */ 
int make_new_seed_mpi ();

/*! \brief Creates and initializes a Sprng object using the parameter values.
 *
 *  The Sprng object is initialized by invoking the initialization method for
 *  the RNG object specified by gtype. If an RNG was already initialized, it will be
 *  freed and substituted by the new one.
 *
 *  \param seed Integral seed value
 *  \param mult Index for multiplier factors (0-NPARAMS)
 *  \param gtype Integer ID for the RNG, default is 0 (LFG)
 *  \return NULL if unsuccessful
 *  \return Garbage value if successful
 */ 
int *init_rng_simple (int seed, int mult, int gtype = 0); 

/*! \brief (MPI) Creates and initializes a Sprng object using the parameter values.
 *
 *  The Sprng object is initialized by invoking the initialization method for
 *  the RNG object specified by gtype. If an RNG was already initialized, it will be
 *  freed and substituted by the new one.
 *
 *  \param seed Integral seed value
 *  \param mult Index for multiplier factors (0-NPARAMS)
 *  \param gtype Integer ID for the RNG, default is 0 (LFG)
 *  \return NULL if unsuccessful
 *  \return Garbage value if successful
 */ 
int *init_rng_simple_mpi (int seed, int mult, int gtype = 0); 

/*! \brief Request an integral random number from RNG.
 *
 *  If an RNG object has not already been initialized, a default Sprng object will be generated
 *  using a seed=0 and mult=0. The RNG object invokes its own method for generating
 *  an integral random number.
 *
 *  \return -1 Create/initialization of RNG failed
 *  \return Integral random number 
 */ 
int get_rn_int_simple ();

/*! \brief (MPI) Request an integral random number from RNG.
 *
 *  If an RNG object has not already been initialized, a default Sprng object will be generated
 *  using a seed=0 and mult=0. The RNG object invokes its own method for generating
 *  an integral random number.
 *
 *  \return -1 Create/initialization of RNG failed
 *  \return Integral random number 
 */ 
int get_rn_int_simple_mpi ();

/*! \brief Request a single-precision floating-point random number from RNG.
 *
 *  If an RNG object has not already been initialized, a default Sprng object will be generated
 *  using a seed=0 and mult=0. The RNG object invokes its own method for generating
 *  a single-precision floating-point random number.
 *
 *  \return -1 Create/initialization of RNG failed
 *  \return Single-precision floating-point random number 
 */ 
float get_rn_flt_simple ();

/*! \brief (MPI) Request a single-precision floating-point random number from RNG.
 *
 *  If an RNG object has not already been initialized, a default Sprng object will be generated
 *  using a seed=0 and mult=0. The RNG object invokes its own method for generating
 *  a single-precision floating-point random number.
 *
 *  \return -1 Create/initialization of RNG failed
 *  \return Single-precision floating-point random number 
 */ 
float get_rn_flt_simple_mpi ();

/*! \brief Request a double-precision floating-point random number from RNG.
 *
 *  If an RNG object has not already been initialized, a default Sprng object will be generated
 *  using a seed=0 and mult=0. The RNG object invokes its own method for generating
 *  a double-precision floating-point random number.
 *
 *  \return -1 Create/initialization of RNG failed
 *  \return Double-precision floating-point random number 
 */ 
double get_rn_dbl_simple ();

/*! \brief (MPI) Request a double-precision floating-point random number from RNG.
 *
 *  If an RNG object has not already been initialized, a default Sprng object will be generated
 *  using a seed=0 and mult=0. The RNG object invokes its own method for generating
 *  a double-precision floating-point random number.
 *
 *  \return -1 Create/initialization of RNG failed
 *  \return Double-precision floating-point random number 
 */ 
double get_rn_dbl_simple_mpi ();

/*! \brief Pack an existing RNG
 *
 *  If an RNG object has not already been initialized, the method fails.
 *  The RNG object invokes its own method for packing generator information.
 *  Buffer parameter needs to be allocated prior to invoking this method.
 *
 *  \param buffer Buffer for storing RNG information
 *  \return 0 Operation failed or RNG has not been initialized 
 *  \return 1 Successful operation 
 */ 
int pack_rng_simple (char **buffer);

/*! \brief Unpack an existing RNG
 *
 *  The method creates a Sprng object based on gtype parameter and invokes
 *  the RNG object invokes its own method for unpacking generator information.
 *  The defaultgen is set with the RNG object created.
 *  Packed parameter needs to be allocated prior to invoking this method.
 *
 *  \param packed Buffer for loading RNG information 
 *  \param gtype Integer ID for the RNG, default is 0 (LFG)
 *  \return NULL if unsuccessful
 *  \return Garbage value if successful
 */
int *unpack_rng_simple (char *packed, int gtype);


int print_rng_simple ();


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
