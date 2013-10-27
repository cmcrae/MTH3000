from metropolis_hastings import metropolis_step
import numpy as np
from numpy.random import randn
import matplotlib.pyplot as plt
from matplotlib.mlab import bivariate_normal

def Q_draw(x, sigma=1.):
	'''
	2D Gaussian random walk with variance sigma^2
	'''
	return x + sigma*randn(2)

def P(x):
	'''
	Target density
	'''
	return 0.5*bivariate_normal(x[0],x[1], mux = -2., muy = 0., sigmaxy = -0.5 )\
		 + 0.5*bivariate_normal(x[0],x[1], mux = 2.,  muy = 0., sigmaxy =  0.5)


def logP(x):
    '''
    Returns the log probability of the target distribution P at x
    '''
    return np.log(P(x))

def metropolis_sampler(x0, num_samples, step_size):
	'''
	Convenience function for performing multiple runs
	of the Metropolis sampler
	'''
	# Array to hold output from sampler
	samples = np.zeros((num_samples,len(x0)))
	# Set starting point to x0
	samples[0] = np.asarray(x0)
	# Run the Metropolis sampler
	for i in range(num_samples-1):
		y = Q_draw(samples[i], step_size) # Draw proposal
		samples[i+1] = metropolis_step(samples[i], y, logP) # Metropolis accept-reject
	return samples

# Simple investigation of mixing using a Metropolis sampler. We perform 4 
# different sample runs with different step sizes and initial positions.
num_runs = 4 # Number of runs
num_samples = 200 # Number of samples to get take for each run
x0 = [ [3,-4], [0,0], [0,0], [3,-4] ] # starting positions bad, good, good, bad
step_size = [ 0.2, 0.2, 1.0 ,1.0] # step sizes: too small, too small, good, good

# Run the samplers
sampler_runs = []
for i in range(num_runs):
	sampler_runs.append( metropolis_sampler(x0[i], num_samples, step_size[i]) )

# Create subplots with each sample run overlayed on the target densities contours
delta = 0.025
x = np.arange(-4., 4., delta)
y = np.arange(-2., 4., delta)
X, Y = np.meshgrid(x, y)
Z = P([X,Y])

fig, ax = plt.subplots(1,num_runs,sharex=True,sharey=True)
for i, run in enumerate(sampler_runs):
	ax[i].plot(run[:,0], run[:,1])
	ax[i].contour(X,Y,Z,colors='k')
	ax[i].set_xticks([]) # Remove x, y ticks
	ax[i].set_yticks([]) 
# Add plot description
ax[0].set_title("Bad starting position\n small step size" ,fontsize=12)
ax[1].set_title("Good starting position\n small step size" ,fontsize=12)
ax[2].set_title("Good starting position\n good step size" ,fontsize=12)
ax[3].set_title("Bad starting position\n good step size" ,fontsize=12)
plt.show()
# plt.plot(sampler_run1[:,0],sampler_run1[:,1])
# plt.plot(sampler_run2[:,0],sampler_run2[:,1]) 
# plt.plot(sampler_run3[:,0],sampler_run3[:,1])

# # Plot histogram of samples
# n,bin,patches = plt.hist(sample,bins=100, normed=1, facecolor='green',alpha=0.5)

# # Overlay plot of unnormalised density
# x = np.linspace(-4,4,1000)
# fx = np.exp(logp(x))
# # Rescale density so that the it has the same maximum as the normalised histogram
# fx /= max(fx)/max(n)
# plt.plot(x, fx, 'k', linewidth=1.5)

plt.ylabel('f(x)',fontsize=12)

# plt.savefig('slice_example.png')