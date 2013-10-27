## Dirichlet Process Gaussian Mixture Model 
C implementation of the Dirichlet Process Gaussian Mixture Model for the old faithful geyser dataset .

### Dependencies
The only dependency is the the MAC OSX accelerate framework. Specifically, the following cblas functions:
`cblas_dscal`, `cblas_daxpy`, and `cblas_dtpsv`, are required by the `crp.c` file.

Plotting is performed by a short python script located in `DPGMM/plots`. To run this script you will need to have python libraries matplotlib and numpy installed. 

### Usage
To run the sampler, cd into the DPGMM folder and compile the source files using:
```
make
```
Assuming everything compiled correctly, you can now  run sampler on the oldfaithful dataset by typing:
```
./test oldfaithful.txt
```
Once sampling has complete, you can create a contour plot of the posterior predictive distribution by running the python file `plotting.py`.

### Acknowledgements
- The Mersenne Twister and Ziggurat code used for random number generation (located in `DPGMM/src/random/`) are modified versions of julia's rng  [source code]( https://github.com/JuliaLang/julia/tree/master/deps/random).
- The linked list code (located in `DPGMM/src/list/`) is based on Exercise 32 of Zed Shaw's book "Learn C The Hard Way" which can be found [here](http://c.learncodethehardway.org/book/ex32.html)

Please refer to websites listed above for copyright and licensing information.
