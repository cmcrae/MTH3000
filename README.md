## Code Descriptions

### DPGMM 
C implementation of the Dirichlet process Gaussian mixture model(DPGMM) for the old faithful geyser dataset. See [1] for details regarding MCMC sampling of Dirichlet process mixture models. Information about Dirichlet process Gaussian mixture models can be found in [2].

### IsingModel
Interactive simulation of the 2D Ising model using the Metropolis algorithm.  A derivation of the Metropolis sampler for the Ising model can be found in Chapter IV Section 31 of [3].

### SliceSample
1-dimensional slice sampler for a multi-modal target distribution. For information on the slice sampler please see [4].

### MetropolisHastings
Python implementation of both Metropolis and Metropolis-Hastings algorithm. The example code plots 200 iterations of the (Gaussian) random walk Metropolis sampler under a variety of initial configurations and step sizes. A mixture of two bivariate Normal distributions is used as the target distribution.


### References
1. Neal, R. M. (2000). Markov chain sampling methods for Dirichlet process mixture models. Journal of computational and graphical statistics, 9(2), 249-265.
2. Görür, D., & Rasmussen, C. E. (2010). Dirichlet process Gaussian mixture models: Choice of the base distribution. Journal of Computer Science and Technology, 25(4), 653-664.
3. MacKay, D. J. (2003). Information theory, inference and learning algorithms. Cambridge university press.
4. Neal, R. M. (2003). Slice sampling. Annals of statistics, 705-741.


