import numpy as np
from numpy.random import rand, exponential, randint

class IsingModel:
	'''
	Rectangular lattice Ising model with periodic boundary conditions
	'''
	def __init__(self, D, T, J, H):
		self.D = D # Height and width of square lattice
		self.T = T # Temperature
		self.J = J # Strength of pairwise interaction
		self.H = H # Strength of external magnetic field
		# Start from random initial state
		self.grid = np.where(rand(D,D)>0.5, 1, -1)
		# Calculate initial energy
		self.E = self.energy()

	def metropolis_step(self):
		'''
		Performs a single Metropolis step for the 2D Ising model 
		on a lattice whose boundaries wrap around and meet (i.e. a torus)
		'''
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
			self.E += dE # Update the current total energy

	def update(self,steps=1):
		'''
		Convenience function to perform multiple Metropolis steps
		'''
		for step in range(steps):
			self.metropolis_step()

	def energy(self):
		'''
		Total energy of the current configuration
		'''
		E = 0
		for i in range(self.D):
			for j in range(self.D):
				E -= self.J * self.grid[i][j] * (self.grid[(i-1)%self.D][j] + self.grid[(i+1)%self.D][j] + self.grid[i][(j-1)%self.D] + self.grid[i][(j+1)%self.D])
				E -= self.H * self.grid[i][j]
		return E

	def set_T(self,T):
		'''
		Sets the temperature to T
		'''
		self.T = T

	def set_J(self,J):
		'''
		Sets the strength of pairwise interaction to J
		'''
		self.J = J

	def set_H(self,H):
		'''
		Sets the strength of external magnetic field to H
		'''
		self.H = H