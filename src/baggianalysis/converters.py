from .core import OxDNAParser, Cogli1Exporter, LAMMPSDataFileParser, LAMMPSDumpParser, Topology, parse_polymer_bondfile, LAMMPSDataFileExporter

import sys, os
from baggianalysis.core import LAMMPSDumpParser


def oxDNA_LAMMPS(input_file, output_file, bond_file=None, datafile_type=None):
    '''Convert an oxDNA configuration into a LAMMPS data file.
    
    Parameters
    ----------
    input_file : str
        The name of the input oxDNA configuration
    output_file : str
        The name of the output LAMMPS data file
    bond_file : str
        The name of the bond file that contains the system's topology
    datafile_type : str
        The type of the LAMMPS data file, which by default is "atomic"
    '''
    parser = OxDNAParser()
    system = parser.make_system(input_file)
    
    if datafile_type is None:
        datafile_type = "atomic"
        if bond_file is not None:
            topology = Topology.make_topology_from_file(bond_file, parse_polymer_bondfile)
            topology.apply(system)
            datafile_type = "bond"
    
    exporter = LAMMPSDataFileExporter(datafile_type)
    exporter.write(system, output_file)


def oxDNA_cogli(input_file, output_file):
    '''Convert an oxDNA configuration into a `cogli <https://sourceforge.net/projects/cogli1/>`_ file.
    
    Parameters
    ----------
    input_file : str
        The name of the input oxDNA configuration
    output_file : str
        The name of the output cogli file
    '''
    parser = OxDNAParser()
    system = parser.make_system(input_file)
    exporter = Cogli1Exporter()
    exporter.write(system, output_file)

    
def LAMMPS_cogli(input_file, output_file):
    '''Convert a LAMMPS data/dump file into a `cogli <https://sourceforge.net/projects/cogli1/>`_ file.
    
    Parameters
    ----------
    input_file : str
        The name of the LAMMPS data file
    output_file : str
        The name of the output cogli file
    '''
    
    # try to understand what kind of input file we are dealing with
    with open(input_file) as f:
        found = False
        for line in f.readlines():
            if line.startswith("ITEM:"):
                input_type = "dump"
                break
            
            N_fields = len(line.split())
            if found and N_fields > 1:
                input_type = "datafile"
                break
            
            if line.strip() == "Atoms":
                found = True
                
    if input_type == "dump":
        parser = LAMMPSDumpParser()
    else:
        if N_fields == 5:
            type = "atomic"
        elif N_fields == 6:
            type = "bond"
        elif N_fields == 7:
            type = "full"
        else:
            print("Unknown data file type: The 'Atoms' section of the data file contains %d fields" % N_fields)
        parser = LAMMPSDataFileParser(type)
            
    system = parser.make_system(input_file)
    exporter = Cogli1Exporter()
    exporter.write(system, output_file)


def oxDNA_LAMMPS_command_line():
    '''Convert the command-line supplied oxDNA configuration into a LAMMPS data file.
    
    The name of the output file is the same as the input file plus '.lammps'.
    '''
    try:
        input_file = sys.argv[1]
    except IndexError:
        print("Usage: is 'oxDNA_cogli1 input_file [bond_file]'", file=sys.stderr)
        exit(1)
        
    bond_file = None
    if len(sys.argv) > 2:
        bond_file = sys.argv[2]
        
    output_file = os.path.basename(input_file) + ".lammps"
    oxDNA_LAMMPS(input_file, output_file, bond_file)

    
def oxDNA_cogli_command_line():
    '''Convert the command-line supplied oxDNA configuration into a `cogli <https://sourceforge.net/projects/cogli1/>`_ file.
    
    The name of the output file is the same as the input file plus '.mgl'.
    '''
    try:
        input_file = sys.argv[1]
    except IndexError:
        print("Usage: is 'oxDNA_cogli1 input_file'", file=sys.stderr)
        exit(1)
        
    output_file = os.path.basename(input_file) + ".mgl"
    oxDNA_cogli(input_file, output_file)

    
def LAMMPS_cogli_command_line():
    '''Convert the command-line supplied LAMMPS data/dump file into a `cogli <https://sourceforge.net/projects/cogli1/>`_ file.
    
    The name of the output file is the same as the input file plus '.mgl'.
    '''
    try:
        input_file = sys.argv[1]
    except IndexError:
        print("Usage: is 'LAMMPS_cogli1 input_file'", file=sys.stderr)
        exit(1)
        
    output_file = os.path.basename(input_file) + ".mgl"
    LAMMPS_cogli(input_file, output_file)
    
