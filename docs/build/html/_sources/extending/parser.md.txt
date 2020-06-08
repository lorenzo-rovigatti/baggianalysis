# Writing a parser

A new baggianalysis parser should inherit from [BaseParser](../core/parsers.html#baggianalysis.core.BaseParser) and overload its [_parse_file(str)](../core/parsers.html#baggianalysis.core.BaseParser._parse_file) method, which takes the name of the configuration file as its only parameter. The new method should initialise a new [System](../core/particles.html#baggianalysis.core.System) and add [Particles](../core/particles.html#baggianalysis.core.Particle) to it. The parser should also set the [box](../core/particles.html#baggianalysis.core.System.box) and the [timestep](../core/particles.html#baggianalysis.core.System.time), but this is not mandatory. 

**Nota Bene:** custom Python parsers cannot be used to generate trajectories from trajectory files through the [initialise_from_trajectory_file()](../core/trajectories.html#baggianalysis.core.BaseTrajectory.initialise_from_trajectory_file) method.

We now write a parser that takes a GROMACS dump file of a [TIP4P/ICE](http://www.sklogwiki.org/SklogWiki/index.php/TIP4P/Ice_model_of_water) system. The system is composed of water molecules made of 4 atoms (one oxygen, two hydrogens and one additional charge site). A configuration file has the following format:

```
water TIP4P/Ice
 1200
    1water  OW1    1   0.127   1.043   1.001
    1water  HW2    2   0.104   1.121   0.950
    1water  HW3    3   0.065   1.043   1.074
    1water  MW4    4   0.115   1.054   1.004
    2water  OW1    5   1.841   0.692   0.574
    2water  HW2    6   1.833   0.600   0.549
    2water  HW3    7   1.933   0.713   0.562
    2water  MW4    8   1.852   0.683   0.569
    ...
   2.05839   2.05839   2.05839
```

where the first line should be skipped, the second line contains the number of atoms and then there are four lines for each molecule, with the three rightmost columns specifying that site's x, y and z coordinates. The last line contains the size of the box edges. 
In this particular file, the time at which the configuration was printed is not stored in the file, but in the name of the file itself, which is sometime linke `QnfT188tXXX`, where `XXX` is the timestep.

Here is the code for the new parser:

	class Tip4pIceParser(ba.BaseParser):
	    def __init__(self):
	        ba.BaseParser.__init__(self)
	    
	    def _parse_file(self, conf):
	        syst = ba.System()
	        
	        with open(conf) as f:
	            # we use the first line to check whether we reached the EOF
	            first_line = f.readline()
	            
	            if len(first_line) == 0:
	                return None
	            
	            syst.time = int(conf.split("t")[1])
	            
	            N_atoms = int(f.readline().strip())
	            for _ in range(N_atoms):
	                line = f.readline()
	                spl = [x.strip() for x in line.split()]
	                if spl[1] == "OW1":
	                    pos = [float(x) for x in spl[3:6]]
	                    particle = ba.Particle(syst.available_index(), "0", pos)
	                    syst.add_particle(particle)
	                    
	            syst.box = [float(x.strip()) for x in f.readline().split()]
	            
	        return syst

Parsers are responsible for constructing valid [Particles](../core/particles.html#baggianalysis.core.Particle), which also means giving new particles valid indexes. Sometimes the indexes are stored in the file to be parsed, but if this is not the case, or if the user does not care about setting indexes manually, parsers can use the [System](../core/particles.html#baggianalysis.core.System)'s utility function `available_index()` to obtain a valid index. 
