#include "random.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h> // timeval struct, gettimeofday
#include "dSFMT.h"    //SIMD-oriented Fast Mersenne Twister
#include "dSFMT.c"



/* ===== Uniform generators ===== */
static double randu_buffer[RANDU_BUFFER_SIZE];  // Array with uniform(0,1) random variates
static int randu_buffer_index = 0; // Index of the next 'fresh' random variate in the buffer

inline uint64_t rand_uint64(void)
{
    double r = dsfmt_gv_genrand_close1_open2();
    return *((uint64_t *) &r) & 0x000fffffffffffff;
}

inline void fill_randu(double array[], int size)
{
  fill_array_open_open(array, size);
}

/* generates a random number on (0,1) with 53-bit resolution */
inline double randu(void)
{
  if (randu_buffer_index > RANDU_BUFFER_SIZE-1) {
    fill_randu(randu_buffer, RANDU_BUFFER_SIZE);
    randu_buffer_index = 0;
  }
  return randu_buffer[randu_buffer_index++];
}

inline void seed_rng(uint32_t seed)
{
 init_gen_rand(seed); 
}

void initialise_rngs(void)
{
  // Initialise the Mersenne twister
  // Seed the rng with the current system time
  struct timeval tv;
  gettimeofday(&tv, NULL);
  seed_rng(tv.tv_sec+tv.tv_usec);
  // Prefill the randu_buffer
  fill_randu(randu_buffer, RANDU_BUFFER_SIZE);
  randu_buffer_index = 0;

  // Initialise Ziggurat tables
  create_ziggurat_tables();
  // And we're done
}


/* ===== Ziggurat normal and exponential generators ===== */
/*
This code is based on the paper Marsaglia and Tsang, "The ziggurat method
for generating random variables", Journ. Statistical Software. Code was
presented in this paper for a Ziggurat of 127 levels and using a 32 bit
integer random number generator. This version of the code, uses the
Mersenne Twister as the integer generator and uses 256 levels in the
Ziggurat. This has several advantages.

  1) As Marsaglia and Tsang themselves states, the more levels the few
     times the expensive tail algorithm must be called
  2) The cycle time of the generator is determined by the integer
     generator, thus the use of a Mersenne Twister for the core random
     generator makes this cycle extremely long.
  3) The license on the original code was unclear, thus rewriting the code
     from the article means we are free of copyright issues.
  4) Compile flag for full 53-bit random mantissa.

It should be stated that the authors made my life easier, by the fact that
the algorithm developed in the text of the article is for a 256 level
ziggurat, even if the code itself isn't...

One modification to the algorithm developed in the article, is that it is
assumed that 0 <= x < Inf, and "unsigned long"s are used, thus resulting in
terms like 2^32 in the code. As the normal distribution is defined between
-Inf < x < Inf, we effectively only have 31 bit integers plus a sign. Thus
in Marsaglia and Tsang, terms like 2^32 become 2^31. We use NOR_MANTISSA for
this term.  The exponential distribution is one sided so we use the
full 32 bits.  We use EXP_MANTISSA for this term.

It appears that I'm slightly slower than the code in the article, this
is partially due to a better generator of random integers than they
use. But might also be that the case of rapid return was optimized by
inlining the relevant code with a #define. As the basic Mersenne
Twister is only 25% faster than this code I suspect that the main
reason is just the use of the Mersenne Twister and not the inlining,
so I'm not going to try and optimize further.
*/

#define ZIGGURAT_TABLE_SIZE 256

#define NOR_MANTISSA 2251799813685248 /* 51 bits for mantissa w/ 1 bit sign */
#define ZIGGURAT_NOR_R 3.6541528853610088
#define ZIGGURAT_NOR_INV_R 0.27366123732975828
#define NOR_SECTION_AREA 0.00492867323399

#define EXP_MANTISSA 4503599627370496  /* 52 bit mantissa */
#define ZIGGURAT_EXP_R 7.69711747013104972
#define ZIGGURAT_EXP_INV_R 0.129918765548341586
#define EXP_SECTION_AREA 0.0039496598225815571993

// Tables for randn
static uint64_t ki[ZIGGURAT_TABLE_SIZE];
static double wi[ZIGGURAT_TABLE_SIZE], fi[ZIGGURAT_TABLE_SIZE];

// Tables for rand_exp
static uint64_t ke[ZIGGURAT_TABLE_SIZE];
static double we[ZIGGURAT_TABLE_SIZE], fe[ZIGGURAT_TABLE_SIZE];


extern void create_ziggurat_tables (void)
{
  int i;
  double x, x1;

  /* Ziggurat tables for the normal distribution */
  x1 = ZIGGURAT_NOR_R;
  wi[255] = x1 / NOR_MANTISSA;
  fi[255] = exp (-0.5 * x1 * x1);

  /* Index zero is special for tail strip, where Marsaglia and Tsang
   * defines this as
   * k_0 = 2^31 * r * f(r) / v, w_0 = 0.5^31 * v / f(r), f_0 = 1,
   * where v is the area of each strip of the ziggurat.
   */
  ki[0] = (uint64_t) (x1 * fi[255] / NOR_SECTION_AREA * NOR_MANTISSA);
  wi[0] = NOR_SECTION_AREA / fi[255] / NOR_MANTISSA;
  fi[0] = 1.;

  for (i = 254; i > 0; i--)
    {
      /* New x is given by x = f^{-1}(v/x_{i+1} + f(x_{i+1})), thus
       * need inverse operator of y = exp(-0.5*x*x) -> x = sqrt(-2*ln(y))
       */
      x = sqrt(-2. * log(NOR_SECTION_AREA / x1 + fi[i+1]));
      ki[i+1] = (uint64_t)(x / x1 * NOR_MANTISSA);
      wi[i] = x / NOR_MANTISSA;
      fi[i] = exp (-0.5 * x * x);
      x1 = x;
    }

  ki[1] = 0;

  /* Zigurrat tables for the exponential distribution */
  x1 = ZIGGURAT_EXP_R;
  we[255] = x1 / EXP_MANTISSA;
  fe[255] = exp (-x1);

  /* Index zero is special for tail strip, where Marsaglia and Tsang
   * defines this as
   * k_0 = 2^32 * r * f(r) / v, w_0 = 0.5^32 * v / f(r), f_0 = 1,
   * where v is the area of each strip of the ziggurat.
   */
  ke[0] = (uint64_t) (x1 * fe[255] / EXP_SECTION_AREA * EXP_MANTISSA);
  we[0] = EXP_SECTION_AREA / fe[255] / EXP_MANTISSA;
  fe[0] = 1.;

  for (i = 254; i > 0; i--)
    {
      /* New x is given by x = f^{-1}(v/x_{i+1} + f(x_{i+1})), thus
       * need inverse operator of y = exp(-x) -> x = -ln(y)
       */
      x = - log(EXP_SECTION_AREA / x1 + fe[i+1]);
      ke[i+1] = (uint64_t)(x / x1 * EXP_MANTISSA);
      we[i] = x / EXP_MANTISSA;
      fe[i] = exp (-x);
      x1 = x;
    }
  ke[1] = 0;
}

/*
 * Here is the guts of the algorithm. As Marsaglia and Tsang state the
 * algorithm in their paper
 *
 * 1) Calculate a random signed integer j and let i be the index
 *     provided by the rightmost 8-bits of j
 * 2) Set x = j * w_i. If j < k_i return x
 * 3) If i = 0, then return x from the tail
 * 4) If [f(x_{i-1}) - f(x_i)] * U < f(x) - f(x_i), return x
 * 5) goto step 1
 *
 * Where f is the functional form of the distribution, which for a normal
 * distribution is exp(-0.5*x*x)
 */

extern double randn(void)
{
  while (1) {
    /* arbitrary mantissa (selected by NRANDI, with 1 bit for sign) */
    const uint64_t r = rand_uint64();
    const int64_t rabs=r>>1;
    const int idx = (int)(rabs&0xFF);
    const double x = ( r&1 ? -rabs : rabs) * wi[idx];
    
    if (rabs < (int64_t)ki[idx]) {
      return x;        /* 99.3% of the time we return here 1st try */
    } 
    else if (idx == 0) {
      /* As stated in Marsaglia and Tsang
       *
       * For the normal tail, the method of Marsaglia[5] provides:
       * generate x = -ln(U_1)/r, y = -ln(U_2), until y+y > x*x,
       * then return r+x. Except that r+x is always in the positive
       * tail!!!! Any thing random might be used to determine the
       * sign, but as we already have r we might as well use it
       *
       * [PAK] but not the bottom 8 bits, since they are all 0 here!
       */
      double xx, yy;
      do {
        xx = - ZIGGURAT_NOR_INV_R * log (randu());
        yy = - log (randu());
      } while ( yy+yy <= xx*xx);
      return (rabs&0x100 ? -ZIGGURAT_NOR_R-xx : ZIGGURAT_NOR_R+xx);
    } 
    else if ((fi[idx-1] - fi[idx]) * randu() + fi[idx] < exp(-0.5*x*x)) {
      return x;
    }
  }
}

extern double rand_exp(void)
{
  while(1) {
    uint64_t ri = rand_uint64();
    const int idx = (int)(ri & 0xFF);
    const double x = ri * we[idx];
    if (ri < ke[idx]) {
      return x;    // 98.9% of the time we return here 1st try
    }
    else if (idx == 0) {
      /* As stated in Marsaglia and Tsang
      * For the exponential tail, the method of Marsaglia[5] provides:
      * x = r - ln(U); */
      return ZIGGURAT_EXP_R - log (randu());
    }
    else if ((fe[idx-1] - fe[idx]) * randu() + fe[idx] < exp(-x)) {
      return x;
    }
  }
}

extern void fill_randn(double array[], int size)
{
  int i;
  for (i = 0; i < size; i++) {
    array[i] = randn();
  }
}

extern void fill_rand_exp(double array[], int size)
{
  int i;
  for (i = 0; i < size; i++) {
    array[i] = rand_exp();
  }
}