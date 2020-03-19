from distutils.core import setup
from __version__ import __version__

import sys
if sys.version_info < (3,0):
    sys.exit('Error, Python < 3.0 is not supported')

setup(
    name = 'baggianalysis',
    version = __version__,
    packages = [ 'baggianalysis' ],
    package_dir = {
        '': '${CMAKE_CURRENT_BINARY_DIR}'
    },
    package_data = {
        '': ['core.so']
    }
)
