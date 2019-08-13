#!/usr/bin/python3

import baggianalysis as ba
import numpy as np
import sys

if len(sys.argv) < 2:
    print("Usage is %s input N_attempts" % sys.argv[0], file=sys.stderr)
    exit(0)
    
parser = ba.LAMMPSDumpParser()
trajectory = ba.FullTrajectory(parser)
trajectory.initialise_from_trajectory_file(sys.argv[1])

obs = ba.PoreSize(int(sys.argv[2]))
results = obs.compute(trajectory)

print("\n".join([str(x) for x in results]))
