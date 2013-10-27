#ifndef _SLICESAMPLE_H
#define _SLICESAMPLE_H

/* Log of unnormalised posterior density for the concentration parameter */
double logp_alpha
(
  double alpha, /* Concentration parameter */
  int k, /* Number of occupied tables */
  int N /* Total number of data points */
);

/* Slice sampling procedure for the concentration parameter.*/
double slice_sample_alpha
(
  double x0, /* Current value of alpha */
  int k, /* Number of occupied tables */
  int N, /* Number of data points */
  double width /* Slice width */
);

#endif