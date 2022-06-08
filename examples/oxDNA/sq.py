import baggianalysis as ba
import sys

if len(sys.argv) < 2:
    print(f"Usage is {sys.argv[0]} trajectory", file=sys.stderr)
    exit(1)
    
parser = ba.OxDNAParser()
trajectory = ba.LazyTrajectory(parser)
trajectory.initialise_from_trajectory_file(sys.argv[1])

sq_obs = ba.StructureFactor(20, 20, 0.01)
sq_obs.analyse_trajectory(trajectory)

for q, Sq in sq_obs.result().items():
    print(q, Sq)
