import numpy as np

def slice_sample(logp, x0, width=1.0, positive=False, eps=1e-10):
    '''
    Slice sampler for 1-dimensional densities

    Parameters
    ----------
    logp : Log density function
    x0 : Initial point
    width: slice width
    positive: True if the density has positive support
    eps: Left endpoint of the interval [eps,infinity).
         Used for densities with positive support.
    '''

    # Evaluate log density at initial position
    logp_x0 = logp(x0)
    # Compute initial slice width and height
    log_height = logp_x0 - np.random.exponential(1.0)
    L = x0 - width*np.random.rand()
    if positive and L<0:
        L = eps
    R = L + width

    # Stepping out procedure with no maximum on the number of steps
    while log_height < logp(L):
            L -= width
            if positive and L<0:
                L = eps
                break

    while log_height < logp(R):
        R += width

    # Sample uniformly under the slice (L,height) x (R,height)
    sample = L + (R-L)*np.random.rand()
    while log_height > logp(sample):
        if sample < x0:
            L = sample
        else:
            R = sample
        sample = L + (R-L)*np.random.rand()
    
    return sample