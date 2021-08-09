#!/usr/bin/env python3

import baggianalysis as ba
import numpy as np

# create a parser to read the LAMMPS data file
parser = ba.LAMMPSDataFileParser("bond")

# parse the system as-is. This particular datafile contains a fully-connected polymer network
full_system = parser.make_system("datafile.lammps")

# the data file contains a configuration of a polymer network, which means that there is a single molecule
print("The data file contains %d molecule(s)" % len(full_system.molecules()))

# exclude crosslinkers, which in this datafile have type 2. Note that in baggianalysis particle types are strings
my_filter = ba.FilterByType(["1",])
chain_system = my_filter.filter(full_system)

# there are 0 chains now, since filters act *after* the original topology is applied
print("The data file contains %d chain(s) [this is not correct, open the file to understand why]" % len(chain_system.molecules()))

# to fix this we first obtain a copy of the original system's topology
topology = ba.Topology.make_topology_from_system(full_system)
# then we remove all the bonds involving the particles that were removed (i.e. the crosslinkers)
topology.remove_unappliable_links(chain_system)
# finally, we apply the topology
topology.apply(chain_system)
print("The data file contains %d chain(s)" % len(chain_system.molecules()))

# now we can do some calculations
average_chain_length = np.average([c.N() for c in chain_system.molecules()])
print("The average chemical length of the chains is %lf" % average_chain_length)
