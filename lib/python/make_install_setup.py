from setuptools import setup

import sys
if sys.version_info < (3, 0):
    sys.exit('Error, Python < 3.0 is not supported')

setup(
    name = 'baggianalysis',
    use_scm_version = {
        "root": "${CMAKE_SOURCE_DIR}",
        "fallback_version": "0.0.1",
        },
    setup_requires = ['setuptools_scm'],
    packages = [ 'baggianalysis' ],
    package_dir = {
        'baggianalysis': '${BA_OUTPUT_DIR}'
    },
    package_data = {
        'baggianalysis': ['core.so']
    }
)
