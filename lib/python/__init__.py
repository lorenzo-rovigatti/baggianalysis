from .core import *

from . import utils

__title__ = 'baggianalysis'
__description__ = 'A C++/Python library to facilitate the analysis of molecular simulations'
__url__ = 'https://github.com/lorenzo-rovigatti/baggianalysis'
__author__ = 'Lorenzo Rovigatti'
__author_email__ = 'lorenzo.rovigatti@uniroma1.it'
__license__ = 'GNU GPL 3.0'
__copyright__ = 'Copyright 2020 Lorenzo Rovigatti'

# automatically retrieve the version
from pkg_resources import get_distribution, DistributionNotFound

try:
    __version__ = get_distribution("baggianalysis").version
except DistributionNotFound:
     # package is not installed
    pass
