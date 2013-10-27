import numpy as np
from numpy.random import rand, exponential, randint
import matplotlib
#matplotlib.use('TkAgg') 
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Slider

class IsingModel:
	def __init__(self, D, T, J, H):
		self.D = D # Height and width of 2D lattice
		self.T = T # Temperature
		self.J = J # Strength of pairwise interaction
		self.H = H # Strength of external magnetic field

		# Start from random initial state
		self.grid = np.where(rand(D,D)>0.5, 1, -1)

	def metropolis_step(self):
		# Select a random spin 
		i = randint(0,self.D)
		j = randint(0,self.D)
		spin_ij = self.grid[i][j]
		# Calculate the change in energy for flipping the spin:
		# Change in energy due to local change in net magnetisation
		dE = 2.0*spin_ij*self.J*(self.grid[(i-1)%self.D][j] + self.grid[(i+1)%self.D][j] + self.grid[i][(j-1)%self.D] + self.grid[i][(j+1)%self.D])
		# Change in energy arising from interaction with external field
		dE += 2.0*spin_ij*self.H

		# If dE <0 then flip it. Otherwise flip it with probability exp(-dE/T)
		if dE/self.T < exponential(1.0):
			self.grid[i][j] *= -1

	def update(self,steps=1):
		for step in range(steps):
			self.metropolis_step()

	def set_T(self,T):
		self.T = T

	def set_J(self,J):
		self.J = J

	def set_H(self,H):
		self.H = H

# Configurable parameters

# Ising model parameters
D = 50
Tmin =  1.0; 	T0 = 3.0; 	Tmax = 5.0;
Jmin = -5.0;  	J0 = 0.0; 	Jmax = 5.0;
Hmin = -5.0;  	H0 = 0.0;	Hmax = 5.0;

# Animation parameters
fps = 30 # Frames per second
spf = 1000 # Metropolis steps per frame update

# Initialise Ising model
model = IsingModel(D, T0, J0, J0)

# Initialise animation
plt.xkcd()
fig, ax = plt.subplots()
plt.subplots_adjust(left=0.2, bottom=0.2)
im = plt.imshow(model.grid, plt.cm.gray)
plt.xkcd()
ax.set_xticks([])
ax.set_yticks([])

# Create sliders to control J, T and H
lightgray = '#f8f8f8'
T_ax =  plt.axes([0.25, 0.15, 0.60, 0.03],axisbg=lightgray)
J_ax  = plt.axes([0.25, 0.10, 0.60, 0.03],axisbg=lightgray)
H_ax  = plt.axes([0.25, 0.05, 0.60, 0.03],axisbg=lightgray)

slider_T = Slider(T_ax, 'T', Tmin, Tmax, valinit=T0)
slider_J = Slider(J_ax, 'J', Jmin, Jmax, valinit=J0)
slider_H = Slider(H_ax, 'H', Hmin, Hmax, valinit=H0)

slider_J.on_changed(model.set_J)
slider_T.on_changed(model.set_T)
slider_H.on_changed(model.set_H)

# Make animation
def init():
	im.set_data(model.grid)
	return [im]

def animate(i):	
	model.update(steps=spf)
	im.set_array(model.grid)
	return [im]


from time import time
t0 = time()
animate(0)
t1 = time()
interval = 1000 * (1./fps) - (t1 - t0)

ani = animation.FuncAnimation(fig, animate, frames=300, interval=interval, blit=False, init_func=init)
plt.show()