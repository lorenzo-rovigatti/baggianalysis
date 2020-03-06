#!/usr/bin/python3

import baggianalysis as ba
import numpy as np
import sys

class MyParser(ba.BaseParser):
    def __init__(self):
        pass
    
    def _parse_file(self, conf):
        syst = ba.System()
        
        with open(conf) as f:
            line = f.readline()
            spl = line.split()
            syst.time = int(spl[0])
            syst.box = [float(x) for x in spl[2:]]
            
            for line in f.readlines():
                pos = [float(x) for x in line.split()]
                particle = ba.Particle("0", pos, [0., 0., 0.])
                syst.add_particle(particle)
            
        return syst
            
    
if len(sys.argv) < 2:
    print("Usage is %s input" % sys.argv[0], file=sys.stderr)
    exit(0)

parser = MyParser()
syst = parser.parse(sys.argv[1])

#nf = ba.CutoffFinder(1.4)
nf = ba.SANNFinder(2.0, ba.SANNFinder.SYMMETRISE_BY_REMOVING)
nf.set_neighbours(syst.particles(), syst.box)

bop_obs = ba.BondOrderParameters({1, 2, 3, 4, 5, 6, 7, 8})
bop_obs.analyse_system(syst)
bops = bop_obs.result()

for i, p_bops in enumerate(bops):
    print("%d %s" % (i, " ".join([str(x) for x in p_bops])))
