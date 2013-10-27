from slice_sample import slice_sample
import numpy as np
import matplotlib.pyplot as plt

def logp(x):
    '''
    Returns the logarithm of the unnormalised density:
    f(x) = ( 1 + sin^2(3x) )( 1 + cos^4(5x) ) exp(-x^2/2)
    '''
    return -0.5* x**2 + np.log(1+np.cos(5*x)**4) + np.log(1+np.sin(3*x)**2)

# Number of samples to generate
N = 15000
# Initial point set to 0
sample = np.zeros(N)
# Slice width
width = 0.2

# Perform slice sampling
for i in range(N-1):
    sample[i+1] = slice_sample(logp, sample[i], width)

# Plot histogram of samples
n,bin,patches = plt.hist(sample,bins=100, normed=1, facecolor='green',alpha=0.5)

# Overlay plot of unnormalised density
x = np.linspace(-4,4,1000)
fx = np.exp(logp(x))
# Rescale density so that the it has the same maximum as the normalised histogram
fx /= max(fx)/max(n)
plt.plot(x, fx, 'k', linewidth=1.5)
plt.xlabel('x',fontsize=12)
plt.ylabel('f(x)',fontsize=12)

plt.savefig('slice_example.png')