
# from IsingModel import IsingModel
# import matplotlib.pyplot as plt
# import numpy as np

# # Initialise Ising model
# D = 250 # Side length of square lattice
# T = 0.1 # Temperature
# J = 1.0 # Strength of pairwise interaction
# H = 0.0 # Strength of external magnetic field
# model = IsingModel(D, T, J, H)

# # Run 
# model.update(500000)

# T = np.linspace(0.1, 33, 100)
# E = np.zeros(100*1000)

# for t in T:
# 	model.set_T(t)
# 	for i in range(1000):
# 		model.metropolis_step() # Run (100D)^2 iterations of Metropolis
# meanE = [ np.mean(E[i:]) for i in range(len(E))]
# varE = [ np.var(E[i:]) for i in range(len(E))]
# plt.errorbar(range(len(E)), meanE, yerr=varE, uplims=upperlimits, lolims=lowerlimits)


# #plt.plot(E)
# #plt.show()
# # # We'll take snapshots of the Ising model at the following temperatures:
# # Temps = [5.0, 2.5, 2.4, 2.3, 2.0]

# # for T in Temps:
# # 	model.set_T(T) # Set temperature
# # 	model.update(steps=D*D) # Run (100D)^2 iterations of Metropolis
# # 	plt.imshow(model.grid, plt.cm.gray) # Plot lattice
# # 	plt.show()