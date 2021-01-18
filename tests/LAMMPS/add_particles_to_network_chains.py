#!/usr/bin/env python3

import baggianalysis as ba
import sys

BI_TYPE = "1"
CL_TYPE = "2"

class Chain():
    def __init__(self, first):
        self.monomers = [first, ]
        self.first = first
        self.last = None

    def append(self, monomer):
        self.monomers.append(monomer)
        self.last = monomer

    def N(self):
        return len(self.monomers) - 1

    def __hash__(self):
        return hash(tuple(sorted(self.monomers)))

    def __eq__(self, other):
        return sorted(self.monomers) == sorted(other.monomers)

    def __ne__(self, other):
        return not __eq__(self, other)

parser = ba.LAMMPSDataFileParser("bond")
system = parser.make_system("initial_conf.dat")
topology = ba.Topology.make_topology_from_file("initial_conf.dat", ba.parse_LAMMPS_topology)
topology.apply(system)
to_be_added = 2

# find the chains
crosslinkers = list(filter(lambda p: p.type == CL_TYPE, system.particles()))
chains = set()
for cl in crosslinkers:
    for neigh in cl.bonded_neighbours:
        prev_neigh = cl
        curr_neigh = neigh
        chain = Chain(cl.index)
        chain.append(curr_neigh.index)
        while curr_neigh.type != "2":
            if len(curr_neigh.bonded_neighbours) != 2:
                print("Crosslinker %d has a chain containing a monomer with %d != 2 neighbour(s)" % (cl.index, len(curr_neigh.bonded_neighbours)))
                break
            for b in curr_neigh.bonded_neighbours:
                if b != prev_neigh:
                    prev_neigh = curr_neigh
                    curr_neigh = b
                    chain.append(curr_neigh.index)
                    break

        # here we avoid double counting chains, while the second condition makes sure that we also add dangling ends
        #if chain.first <= chain.last or curr_neigh.type == "1":
        if curr_neigh.type == "2" and chain.first != chain.last:
            chains.add(chain)

print("%d chains found" % len(chains))

last_idx = system.particles()[-1].index

exporter = ba.LAMMPSDataFileExporter("bond")
exporter.write(system, "final_conf.dat")
