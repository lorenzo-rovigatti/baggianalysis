## What should `baggianalysis` be able to do?

1. Load trajectories of any kind
	* spherical and anisotropic
	* single file or split into separate files
	* different box types
2. Standard analysis
	* S(q), g(r)
	* MSD, density-density correlation functions, Van Hove
3. Standard analysis on subsets of the system
	* only certain particles types
	* only particles that have particular characteristics (specific ranges of positions, orientations, *etc.*)
4. Manipulation of the system itself (so that the output would be a new configuration in some format)
	* rotate a portion of the system
	* remove some of the particles
	* add more particles ?

Python bindings would be greatly beneficial towards points 3. and 4.

## How?

* Configurations of different formats need to be translated into an agnostic format. This format needs to be able to manage all the details of the starting configuration
	* What about the topology? How do we store it?
* We need Python bindings (see points 3. and 4. of the previous section):
```python
import baggianalysis as ba

traj = ba.load("trajectory.dat", format="LAMMPS")
traj_A_only = traj.take(type="A")
traj_small_x_only = traj.take(???)
```
* We can add heuristics to make baggianalysis understand what kind of configurations we are dealing with

## Architecture

* The System class should be a template of at least the type of particles (point particles, anisotropic particles, rigid bodies, *etc.*).
