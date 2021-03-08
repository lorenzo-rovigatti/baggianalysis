# Configuration parsers

All parsers inherit from [BaseParser](#baggianalysis.core.BaseParser) and override the `_parse_stream` and, possibly, the `_parse_file` methods. After a parser has been initialised it can be used to build a single system through its [make_system()](#baggianalysis.core.BaseParser.make_system) method or a whole trajectory by using it to initialise a [Trajectory](trajectories.md) object.

```eval_rst
.. currentmodule:: baggianalysis.core

.. autosummary::
    :nosignatures:

    BaseParser
    OxDNAParser
    GroParser
    LAMMPSDataFileParser
    LAMMPSDumpParser
    LJKAParser
    PatchyParticlesParser
    
.. autoclass:: BaseParser
    :private-members:
    
.. autoclass:: OxDNAParser

.. autoclass:: baggianalysis.core.oxDNA_topology.Default

.. autoclass:: baggianalysis.core.oxDNA_topology.TSP
    
.. autoclass:: GroParser

.. autoclass:: LAMMPSDataFileParser
    
.. autoclass:: LAMMPSDumpParser
    
.. autoclass:: LJKAParser

.. autoclass:: PatchyParticlesParser
```
