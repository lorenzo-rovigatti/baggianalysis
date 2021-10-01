from .core import OxDNAParser, Cogli1Exporter

import sys, os

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
    