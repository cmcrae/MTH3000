#ifndef _RANDOM_H
#define _RANDOM_H

#include <stdint.h> // uint64_t, uint32_t

/* The buffer for increasing dSFMT calling efficiency.	*/
#define RANDU_BUFFER_SIZE 65536

/* Seeds the dSFMT rng. */
void seed_rng(uint32_t seed);

/* Creats ziggurat tables for normal and exponential */
void create_ziggurat_tables(void);

/* Convenience function used for initialising both dSFMT 
 and ziggurat rngs. Uses system time to seed the dSFMT. */
void initialise_rngs(void);

/* Uniform(0,1) random variables with dSFMT. */
double randu(void);

/* Fills array with uniform(0,1) random variates */
void fill_randu(double array[], int size);

/* Generates random unsigned 64-bit integers */
uint64_t rand_uint64(void);

/* Normal(0,1) random variables using ziggurat method. */
double randn(void);

/* Fills array with normal(0,1) random variates */
void fill_randn(double array[], int size);

/* Exponential(1) random variables using ziggurat method. */
double rand_exp(void);

/* Fills array with exponential(1) random variates */
void fill_rand_exp(double array[], int size);

#endif