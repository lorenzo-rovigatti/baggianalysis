#!/usr/bin/env python3

import numpy as np
import baggianalysis as ba

LIMIT = 1e-6

parser = ba.GenericOxDNAParser("topology.dat")
trajectory = ba.LazyTrajectory(parser)
trajectory.initialise_from_trajectory_file("trajectory.dat")

msd_obs = ba.MSD(trajectory)
msd_obs.compute_and_print(20, False, "msd.dat")

exit(0)
