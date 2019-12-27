#!/usr/bin/python3

import baggianalysis as ba
import numpy as np
import sys

if len(sys.argv) < 3:
    print("Usage is %s topology configuration" % sys.argv[0], file=sys.stderr)
    exit(1)
    
parser = ba.GenericOxDNAParser(sys.argv[1])
trajectory = ba.FullTrajectory(parser)
trajectory.add_filter(ba.FilterByFunction(lambda x: x.position[0] < 0))
trajectory.initialise_from_trajectory_file(sys.argv[2])

syst = trajectory.next_frame()
print(syst.N())
