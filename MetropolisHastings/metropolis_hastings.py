import numpy as np
from numpy.random import rand

def metropolis_hastings_step(x,y,logQ,logP):
    '''
    Performs a single Metropolis Hastings update 
    given the current x and the proposal y.

    Parameters
    ----------
    x : scalar or array
        Current state of Markov chain.
    y : scalar or array
        Proposal drawn from the proposal distribution, Q(.|x)
    logQ(x,y) : Function 
        Computes log Q(x|y) i.e. the log probability of x 
        given y using the proposal distribution.
    logP(x) : Function
        Computes log P(x)  i.e. the log probability target
        distribution P at x
    '''

    # Calculate the log of the Metropolis-Hastings ratio 
    log_ratio = logP(y) - logP(x) + logQ(y,x) - logQ(y,x)

    # Accept/reject step
    if isfinite(log_ratio) and log(rand()) < log_ratio:
        return y
    else:
        return x

def metropolis_step(x,y,logP):
    '''
    Performs a single Metropolis update given the 
    current x and the proposal y. This is just
    Metropolis-Hastings with a symmetric proposal distribution
    i.e. Q(x|y) = Q(y|x)

    Parameters
    ----------
    x : scalar or array
        Current state of Markov chain.
    y : scalar or array
        Proposal drawn from the proposal distribution, Q(.|x)
    logP(x) : Function
        Computes log P(x)  i.e. the log probability target
        distribution P at x
    '''

    # Calculate the log of the Metropolis ratio 
    log_ratio = logP(y) - logP(x)

    # Accept/reject step
    if np.isfinite(log_ratio) and np.log(rand()) < log_ratio:
        return y
    else:
        return x