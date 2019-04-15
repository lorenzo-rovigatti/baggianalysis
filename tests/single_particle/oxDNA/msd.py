import numpy as np
import baggianalysis as ba

parser = ba.OxDNAParser("topology.dat")
trajectory = ba.LazyTrajectory(parser)
trajectory.initialise_from_trajectory_file("trajectory.dat")

msd = ba.MSD(trajectory)
msd.compute_and_print(20, False, "msd.dat")
