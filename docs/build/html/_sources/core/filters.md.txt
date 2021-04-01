# Filters

Filters take a system as input and generate a new system according to some user-defined criteria. There are filters that remove particles of a given type (or types), filters that remove the centre-of-mass-position from the particles, filters that include or exclude particles according to a user-defined function and more.

Once a filter has been created, it can be applied to single systems like this::

    new_system = my_filter.filter(old_system)
    
In addition, filters can be applied to whole trajectories by using the BaseTrajectory's [add_filter()](../core/trajectories.html#baggianalysis.core.BaseTrajectory.add_filter) method before these are parsed.

```eval_rst
.. currentmodule:: baggianalysis.core

.. autosummary::
    :nosignatures:

    BaseFilter
    FilterByFunction
    FilterById
    FilterByReducingToCOM
    FilterByType
    FixParticlePath
    MapParticles
    MolecularCOMs
    SubtractCOM
    
.. autoclass:: BaseFilter
    
.. autoclass:: FilterByFunction

.. autoclass:: FilterById
    
.. autoclass:: FilterByReducingToCOM
    
.. autoclass:: FilterByType
    
.. autoclass:: FixParticlePath

.. autoclass:: MapParticles
    
.. autoclass:: SubtractCOM
```
