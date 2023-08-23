# The converters module

This module contains some high-level utility functions to interconvert between different file formats. When installed
it also creates the following standalone executable scripts:

* `oxDNA_LAMMPS input_file [bond_file]`, which calls {func}`~baggianalysis.converters.oxDNA_LAMMPS_command_line()`.
* `oxDNA_cogli input_file`, which calls {func}`~baggianalysis.converters.oxDNA_cogli_command_line()`.
* `LAMMPS_cogli input_file`, which calls {func}`~baggianalysis.converters.LAMMPS_cogli_command_line()`.

```eval_rst
.. toctree::
   :maxdepth: 2

.. currentmodule:: baggianalysis.converters

.. autosummary::
    :nosignatures:
    
    oxDNA_LAMMPS
    oxDNA_LAMMPS_command_line

    oxDNA_cogli
    oxDNA_cogli_command_line
    
    LAMMPS_cogli
    LAMMPS_cogli_command_line

.. autofunction:: oxDNA_LAMMPS
.. autofunction:: oxDNA_LAMMPS_command_line
    
.. autofunction:: oxDNA_cogli
.. autofunction:: oxDNA_cogli_command_line

.. autofunction:: LAMMPS_cogli
.. autofunction:: LAMMPS_cogli_command_line
    
```
