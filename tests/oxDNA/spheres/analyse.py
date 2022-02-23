#!/usr/bin/env python3

import baggianalysis as ba
from collections import Counter

parser = ba.OxDNAParser()
system = parser.make_system("configuration.dat")

finder = ba.CutoffFinder(2.0)
finder.set_neighbours(system.particles(), system.box)
obs = ba.ClusterList()
obs.analyse_system(system)
sizes = obs.result()

with open("csd.dat", "w") as f:
    histogram = Counter(sizes)
    for k, v in sorted(histogram.items()):
        print(k, v, file=f)

exporter = ba.Cogli1Exporter()
exporter.write(system, "configuration.mgl")
