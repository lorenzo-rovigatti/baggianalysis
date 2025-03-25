from .core import FullTrajectory, LazyTrajectory, LAMMPSDumpParser, OxDNAParser

def _trajectory(parser_name, parser_kwargs={}, type="Full", filters=[]):
    parser = globals()[parser_name](**parser_kwargs)
    if type == "Full":
        trajectory = FullTrajectory(parser)
    elif type == "Lazy":
        trajectory = LazyTrajectory(parser)
    else:
        raise Exception("Invalid trajectory type '%s'" % type)
    
    for f in filters:
        trajectory.add_filter(f)
        
    return trajectory

def LAMMPSTrajectory(data_file=None, atom_style=None, type="Full", filters=[]):
    '''Create and return a LAMMPS trajectory.
    
    The resulting trajectory needs to be explicitly initialised by the user with one of 
    :class:`~.BaseTrajectory`'s ``initialise_from_*`` methods.
    
    Parameters
    ----------
    data_file : str
        The name of an optional data file that will be used to initialise the trajectory's frames. This is 
        useful if the LAMMPS dump files do not contain all the information (*e.g.* charges) or if you want 
        frames to also store the particle connectivity (*e.g.* bonds, angles and dihedrals).  
    atom_tyle : str
        The atom style of the data file. Note that this parameter is required if data_file is not None.
    type : str
        The type of trajectory, which should be either "Lazy" or "Full" (for :class:`~.LazyTrajectory` and 
        :class:`~.FullTrajectory`, respectively)'.
    filters : list
        A list of filters that should be added to the trajectory. Note that the order counts, since filters 
        are applied on a first-come first-served basis.
            
    Returns
    -------
    :class:`~.LazyTrajectory` or :class:`~.FullTrajectory`
        A trajectory object. Note that the trajectory as returned by this function still requires initialisation 
        through one of :class:`~.BaseTrajectory`'s ``initialise_from_*`` methods.
    '''
    
    parser_args = {}
    if data_file is not None:
        if atom_style is None:
            raise Exception("Initialising the trajectory with a LAMMPS data file requires the atom_style parameter")
        parser_args["data_file"] = data_file
        parser_args["atom_style"] = atom_style
        
    return _trajectory("LAMMPSDumpParser", parser_kwargs=parser_args, type=type, filters=filters)

def OxDNATrajectory(topology_file, type="Full", filters=[]):
    '''Create and return an oxDNA trajectory.
    
    The resulting trajectory needs to be explicitly initialised by the user with one of 
    :class:`~.BaseTrajectory`'s ``initialise_from_*`` methods.
    
    Parameters
    ----------
    topology_file : str
        The topology file associated to the oxDNA trajectory.
    type : str
        The type of trajectory, which should be either "Lazy" or "Full" (for :class:`~.LazyTrajectory` and 
        :class:`~.FullTrajectory`, respectively)'.
    filters : list
        A list of filters that should be added to the trajectory. Note that the order counts, since filters 
        are applied on a first-come first-served basis.
            
    Returns
    -------
    :class:`~.LazyTrajectory` or :class:`~.FullTrajectory`
        A trajectory object. Note that the trajectory as returned by this function still requires initialisation 
        through one of :class:`~.BaseTrajectory`'s ``initialise_from_*`` methods.
    '''
    parser_args = {
        "topology_file": topology_file
        }
    return _trajectory("OxDNAParser", parser_kwargs=parser_args, type=type, filters=filters)
