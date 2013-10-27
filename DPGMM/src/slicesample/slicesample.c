#include <float.h> // DBL_EPSILON, INFINITY
#include "../random/random.h" // randu, rand_exp
#include <math.h> // log, lgamma
#include "slicesample.h"

/* Log of unnormalised posterior density for the concentration parameter */
double logp_alpha(double alpha, int k, int N)
{
	if(0 < alpha && alpha < 100*N) {
		return (k-1.5)*log(alpha) - 0.5*alpha + lgamma(alpha) - lgamma(N+alpha);
	} else {
		return -INFINITY;
	}
}

/* Slice sampling procedure for the concentration parameter.*/
double slice_sample_alpha(double x0, int k, int N, double width)
{
	double result; // New sample to be returned
	double logp_x0 = logp_alpha(x0,k,N); // Log density at initial point
	double log_height = logp_x0 - rand_exp(); // Slice height in log terms
	
	// Compute initali slice width: R - L
	double R = x0 + width*randu();
	double L = R - width;
	
	if(L<0) { /* alpha has a postive support */
		L = DBL_EPSILON; /* Machine epsilon */
	}
	

	/* Stepping out procedure (No maximum on the number of steps) */
	while(log_height < logp_alpha(L,k,N)) {
		L -= width;
		if(L<0) {
			L = DBL_EPSILON;
			break;
		}
	}
	while(log_height < logp_alpha(R,k,N))
		R += width;
	
	/* Sample from the slice, stepping in if necessary */
	result = L + (R-L)*randu();
	while(logp_alpha(result,k,N) < log_height) {
		if(result < x0) {
			L = result;
		} else {
			R = result;
		}
		result = L + (R-L)*randu();
	}
	return result;
}

// int main(void)
// {
// 	initialise_rngs();
// 	int k = 100;
// 	int N = 100000;
// 	double alpha = 0.5;
// 	int i;
// 	printf("P(a) = %f \n", logp_alpha(alpha,k,N));
// 	for(i=0; i<N; i++) {
// 		alpha = slice_sample_alpha(alpha,k,N,1.0);
// 		printf("alpha = %e \n", alpha);
// 	}
// 	return 0;
// }