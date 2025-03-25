from .core import *

from . import utils, traj, converters

__title__ = 'baggianalysis'
__description__ = 'A C++/Python library to facilitate the analysis of molecular simulations'
__url__ = 'https://github.com/lorenzo-rovigatti/baggianalysis'
__author__ = 'Lorenzo Rovigatti'
__author_email__ = 'lorenzo.rovigatti@uniroma1.it'
__license__ = 'GNU GPL 3.0'
__copyright__ = 'Copyright 2020 Lorenzo Rovigatti'

try:
    from ._version import version as __version__
except ImportError:
    __version__ = "unknown"
    