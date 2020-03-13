# Configuration parsers

All parsers inherit from [BaseParser](#baggianalysis.core.BaseParser) and override the `_parse_stream` and, possibly, the `_parse_file` methods. After a parser has been initialised it can be used to build a single system through its [make_system()](#baggianalysis.core.BaseParser.make_system) method or a whole trajectory by using it to initialise a [Trajectory](trajectories.md) object.

```eval_rst
.. currentmodule:: baggianalysis.core

.. autosummary::
    :nosignatures:

    BaseParser
    GenericOxDNAParser
    GroParser
    LAMMPSDataFileParser
    LAMMPSDumpParser
    LJKAParser
    
.. autoclass:: BaseParser
    :members:
    :undoc-members:
    
.. autoclass:: GenericOxDNAParser
    :members:
    :undoc-members:

.. autoclass:: GroParser
    :members:
    :undoc-members:

.. autoclass:: LAMMPSDataFileParser
    :members:
    :undoc-members:
    
.. autoclass:: LAMMPSDumpParser
    :members:
    :undoc-members:
    
.. autoclass:: LJKAParser
    :members:
    :undoc-members:
```
