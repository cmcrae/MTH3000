
from IsingModel import IsingModel
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Slider

# Configurable parameters
D = 50 # Side length of square lattice
Tmin =  1.0; 	T0 = 3.0; 	Tmax = 5.0; # Temperature
Jmin = -5.0;  	J0 = 0.0; 	Jmax = 5.0; # Strength of pairwise interaction
Hmin = -5.0;  	H0 = 0.0;	Hmax = 5.0; # Strength of external magnetic field

# Animation parameters
fps = 30 # Frames per second
spf = 1000 # Metropolis steps per frame update

# Initialise Ising model
model = IsingModel(D, T0, J0, H0)

# Initialise animation
fig, ax = plt.subplots()
plt.subplots_adjust(left=0.2, bottom=0.2)
im = plt.imshow(model.grid, plt.cm.gray)
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

# Run animation
from time import time
t0 = time()
animate(0)
t1 = time()
interval = 1000 * (1./fps) - (t1 - t0)

ani = animation.FuncAnimation(fig, animate, frames=300, interval=interval, blit=False, init_func=init)
plt.show()