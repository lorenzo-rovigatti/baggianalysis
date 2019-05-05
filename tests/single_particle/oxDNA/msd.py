#!/usr/bin/env python3

import numpy as np
import baggianalysis as ba

LIMIT = 1e-6

parser = ba.OxDNAParser("topology.dat")
trajectory = ba.LazyTrajectory(parser)
trajectory.initialise_from_trajectory_file("trajectory.dat")

msd_obs = ba.MSD(trajectory)
msd_obs.compute_and_print(20, False, "msd.dat")

exit(0)

msd = msd_obs.compute(20, False)

x_axis = np.fromiter(msd.keys(), dtype=np.int32)
y_axis = np.fromiter(msd.values(), dtype=np.float64)

ref_x_axis = np.genfromtxt("msd.dat", np.int32, usecols=(0, ))
ref_y_axis = np.genfromtxt("msd.dat", np.float64, usecols=(1, ))

try:
    x_chi = np.sum((x_axis - ref_x_axis)**2. / ref_x_axis)
    y_chi = np.sum((y_axis - ref_y_axis)**2. / ref_y_axis)
except ValueError as e:
    print("TEST FAILED: %s" % str(e), file=sys.stderr)
    exit(1)
        
if x_chi > LIMIT:
    print("TEST FAILED: the chi square for the x axis exceeds the test threshold (%f > %f)" % (x_chi, LIMIT))
    exit(1)
    
if y_chi > LIMIT:
    print("TEST FAILED: the chi square for the y axis exceeds the test threshold (%f > %f)" % (y_chi, LIMIT))
    exit(1)
    
print("TEST PASSED")
