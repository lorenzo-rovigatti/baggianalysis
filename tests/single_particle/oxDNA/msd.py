import numpy as np
import baggianalysis as ba

parser = ba.OxDNAParser("topology.dat")
trajectory = ba.LazyTrajectory(parser)
trajectory.initialise_from_trajectory_file("trajectory.dat")

msd = ba.MSD(trajectory)
options = msd.default_options()
options.points_per_cycle = 20
msd.compute_and_print(options)
