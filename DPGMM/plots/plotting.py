import numpy as np
from matplotlib import colors, ticker, cm
import matplotlib.pyplot as plt

# Load meshgrid
x = np.loadtxt('xgrid.txt')
y = np.loadtxt('ygrid.txt')

# Load posterior predictive evaluations on meshgrid
post_pred_surf = np.loadtxt('post_pred_surf.txt')
# Normalise posterior predictive distribution over the meshgrid
post_pred_surf /= sum(sum(post_pred_surf))

# Create contour plot of posterior predictive distribution
# Note the colours use a logarithmic scale
plt.contourf(x,y,post_pred_surf, locator=ticker.LogLocator(), cmap=cm.PuBu_r)
# Add a colour bar for reference
plt.colorbar()

# Load the old faithful data set
oldfaithful_data = np.loadtxt('../oldfaithful.txt')
eruptions = oldfaithful_data[:,0]
waitingtime = oldfaithful_data[:,1]

# Overlay raw data on contour plot
plt.plot(eruptions, waitingtime,'ko')
plt.xlabel('Eruption time (min)',fontsize=12)
plt.ylabel('Waiting time to next eruption (min)',fontsize=12)

plt.savefig('oldfaithful_predpost.png')
