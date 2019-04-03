import numpy as np
import baggianalysis as ba

parser = ba.OxDNAParser("topology.dat")
trajectory = ba.LazyTrajectory(parser)
trajectory.initialise_from_trajectory_file("trajectory.dat")

system = trajectory.next_frame()

poss = system.particles.positions

v = np.array([1., 0., 1.])
#system.particles.positions[0] = v

poss.append(v)

print(poss)
print(system.particles.positions)
