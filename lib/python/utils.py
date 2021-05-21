'''
A collection of utility functions
'''

def print_bondfile(filename, system, shift_indexes=False):
    '''Print the bondfile relative to the given system. The bondfile will contain 2N lines, where N is the number of particles.
    
    Parameters
    ----------
        filename : str
            The name of the file that will be printed
        system : :class:`System`
            The system whose bonds will be printed
        shift_indexes : bool
            Whether indexes should be shifted by one. This is used to transform zero-based indexes to one-based for e.g. Fortran compatibility. Defaults to False.
    '''
    system.sort_particles_by_id()

    if shift_indexes:
        shift = 1
    else:
        shift = 0
    
    with open(filename, "w") as out:
        for p in system.particles():
            print(len(p.bonded_neighbours), file=out)
            print(" ".join(str(bond.other().index + shift) for bond in p.bonded_neighbours), file=out)
            