#!/usr/bin/env python3

import numpy as np
import baggianalysis as ba

parser = ba.OxDNAParser("topology.dat")
trajectory = ba.LazyTrajectory(parser)
trajectory.initialise_from_trajectory_file("trajectory.dat")

msd_obs = ba.MSD(20, False)
msd_obs.analyse_and_print(trajectory, "msd.dat")

exit(0)
