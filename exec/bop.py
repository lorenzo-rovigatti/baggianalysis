#!/usr/bin/python3

import baggianalysis as ba
import numpy as np
import sys

class MyParser(ba.BaseParser):
    def __init__(self):
        pass
    
    def parse(self, conf):
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
            
    

parser = MyParser()
syst = parser.parse(sys.argv[1])

nf = ba.CutoffFinder(1.4)
nf.set_neighbours(syst.particles(), syst.box)

bop_obs = ba.BondOrderParameters({1, 2, 3, 4, 5, 6, 7, 8})
bops = bop_obs.compute(syst)

for i, p_bops in enumerate(bops):
    print(str(i) + " " + " ".join([str(x) for x in p_bops]))
